#ifdef HC_WITH_KOKKOS
#include "asyncKokkosEncoder.hpp"
#include <fstream>
#include <ranges>
#include <algorithm>
#include <thread>

void AsyncKokkosEncoder::init(){
    auto counts = asyncInit(filename_);
    buildFromFreq(counts);
}

void AsyncKokkosEncoder::readThread(std::array<std::string, 256>& codes){
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

void AsyncKokkosEncoder::compressThread() {
    bool last = false;
    size_t compressedFileLen = 0;
    while (!last){
        auto [compressedString, lastMsg] = *compressQueue_.wait_and_pop();
        last = lastMsg;
        size_t nbytes = compressedString.size();
        // Kokkos::View<char*, Kokkos::DefaultExecutionSpace> v;
        if (compressedString.size() % 8 != 0){
            std::cout << "Error: Compressed string must be a multiple of 8 bytes long" << std::endl;
        }
        Kokkos::View<char*, Kokkos::HostSpace, Kokkos::MemoryUnmanaged> stringView(compressedString.data(), compressedString.size());
        Kokkos::View<char*, Kokkos::DefaultExecutionSpace> d_stringView("string view device", nbytes);
        Kokkos::deep_copy(d_stringView, stringView);
        Kokkos::View<unsigned char*, Kokkos::DefaultExecutionSpace> d_bytesView("bytes view device", nbytes / 8 );

        // Compress down to bytes asynchronously. Allocate Memory in the mean time. 
        Kokkos::parallel_for("Convert to bytes", nbytes / 8, KOKKOS_LAMBDA(const uint32_t i){
            unsigned char ch = 0;
            for (size_t b = 0; b < 8; ++b){
                ch = (ch << 1) | (d_stringView[8*i + b] == 49); // 49 is ascii for "1"
            }
            d_bytesView(i) = ch;
        });
        // Overlap final allocation with end of data. 
        std::vector<unsigned char> compressedChars(nbytes / 8);
        Kokkos::View<unsigned char*, Kokkos::HostSpace, Kokkos::MemoryUnmanaged> bytesView(compressedChars.data(), nbytes/8);

        // Wait for the kernel to be done. 
        Kokkos::DefaultExecutionSpace().fence();
        Kokkos::deep_copy(bytesView, d_bytesView); // D2H copy. 
        writeQueue_.push({compressedChars, last});
        compressedFileLen += compressedChars.size();
   }
}

void AsyncKokkosEncoder::writeThread(){
    bool last = false;
    std::ofstream out{filename_ + ".compress", std::ios::out};
    while (!last){
        auto [buffer, lastMsg] = *writeQueue_.wait_and_pop();
        last = lastMsg;
        out.write((char*) buffer.data(), buffer.size());
    }
}

void AsyncKokkosEncoder::writeToFile(std::array<std::string, 256>& codes){
    std::thread read([this, &codes](){return readThread(codes);});
    std::thread write([this](){writeThread();});

    compressThread();
    read.join();
    write.join();
}

#endif