#include "AsyncEncoder.hpp"

#include <fstream>
#include <thread>
#include <future>

AsyncEncoder::AsyncEncoder(const std::array<std::string, 256>& codes, std::string filename):
    codes_{codes}, filename_{filename}{}

size_t AsyncEncoder::readThread(){
    std::fstream input{filename_, std::ios::in};
    unsigned char c;
    if (!input.is_open()){
        std::cerr << "Unable to read file" << std::endl;
        compressQueue_.push({"", true});
        return 0;
    }
    // read into 1 MB chunks
    const size_t chunkSize = 1024 * 1024;
    std::vector<unsigned char> filechunk(chunkSize);
    std::string extraBytes;
    while (true){
        std::string compressedString = extraBytes;
        input.read((char*)filechunk.data(), chunkSize);
        std::streamsize dataread = input.gcount();

        for (size_t i = 0; i < dataread; ++i){
            unsigned char c = filechunk[i];
            compressedString.append(codes_[c]);
        }
        if (dataread < chunkSize){ // last one
            // Pad the rest of the values with ones. 
            while (compressedString.length() % 8 != 0){
                compressedString.append("1");
            }
            compressQueue_.push({compressedString, true});
            break;
        } else {
            size_t extra = compressedString.size() % 8;
            std::copy(compressedString.end() - extra, compressedString.end(), extraBytes.begin());
            compressedString.erase(compressedString.length() - extra);
            compressQueue_.push({compressedString, false});
        }
    }
    return std::filesystem::file_size(filename_);
}

size_t AsyncEncoder::compressThread(){
    bool last = false;
    size_t compressedFileLen = 0;
    while (!last){
        auto [compressedString, lastMsg] = *compressQueue_.wait_and_pop();
        last = lastMsg;

        if (compressedString.size() % 8 != 0){
            std::cout << "Error: Compressed string must be a multiple of 8 bytes long" << std::endl;
        }
        std::vector<unsigned char> compressedChars;
        compressedChars.reserve(compressedString.size() / 8);
        std::string::iterator it = begin(compressedString);
        while(it != end(compressedString)){
            unsigned char ch = 0;
            for (size_t i = 0; i < 8; ++i){
                ch = (ch << 1) | (*it == 49);
                ++it;
            }
            compressedChars.push_back(ch);
        }
        writeQueue_.push({compressedChars, last});
        compressedFileLen += compressedChars.size();
   }
   return compressedFileLen;
}

void AsyncEncoder::writeThread(){
    bool last = false;
    std::ofstream out{filename_ + ".compress", std::ios::out};
    // std::vector<unsigned char> buffer;
    while (!last){
        auto [buffer, lastMsg] = *writeQueue_.wait_and_pop();
        last = lastMsg;
        out.write((char*) buffer.data(), buffer.size());
    }
}

std::pair<size_t, size_t> AsyncEncoder::launch(){
    std::future<size_t> fileLenFuture = std::async([this](){return readThread();});
    std::future<size_t> compLenFuture = std::async([this](){return compressThread();});
    std::thread write = std::thread([this](){writeThread();});

    size_t filelen = fileLenFuture.get();
    size_t compfilelen = compLenFuture.get();
    write.join();
    return std::make_pair(filelen, compfilelen);
}
