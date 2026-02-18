#include "asyncEncoder.hpp"

#include <fstream>
#include <thread>
#include <future>

void AsyncEncoder::init() {

    std::array<unsigned long, 256> counts = asyncInit(filename_);
    buildFromFreq(counts);
}

void AsyncEncoder::readThread(std::array<std::string, 256>& codes){
    std::fstream input{filename_, std::ios::in};
    if (!input.is_open()){
        std::cerr << "Unable to read file" << std::endl;
        compressQueue_.push({"", true});
    }
    // read into 1 MB chunks
    std::vector<unsigned char> filechunk(chunkSize_);
    std::string extraBytes;
    while (true){
        std::string compressedString = extraBytes;
        input.read((char*)filechunk.data(), chunkSize_);
        std::streamsize dataread = input.gcount();

        for (size_t i = 0; i < dataread; ++i){
            unsigned char c = filechunk[i];
            compressedString.append(codes[c]);
        }
        if (dataread < chunkSize_){ // last one
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
}

void AsyncEncoder::compressThread(){
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
}

void AsyncEncoder::writeThread(){
    bool last = false;
    std::ofstream out{filename_ + ".compress", std::ios::out};
    while (!last){
        auto [buffer, lastMsg] = *writeQueue_.wait_and_pop();
        last = lastMsg;
        out.write((char*) buffer.data(), buffer.size());
    }
}


void AsyncEncoder::writeToFile(std::array<std::string, 256>& codes){
    std::thread read([this, &codes](){return readThread(codes);});
    std::thread compress([this](){return compressThread();});
    std::thread write([this](){writeThread();});

    read.join();
    compress.join();
    write.join();
}