
#include "asyncMixin.hpp"
#include <fstream>
#include <thread>
#include <array>

std::array<unsigned long, 256> AsyncMixin::asyncInit(std::string filename){
    std::fstream input{filename, std::ios::in};
    if (!input.is_open()){
        std::cerr << "Unable to read file" << std::endl;
        return {};
    }
    std::array<unsigned long, 256> counts;
    std::thread count([this, &counts](){countThread(counts);});
    while(true){
        std::vector<unsigned char> filechunk(chunkSize_);
        input.read((char*)filechunk.data(), chunkSize_);
        std::streamsize dataread = input.gcount();
        filechunk.resize(dataread);
        readQueue_.push(filechunk);
        if (dataread != chunkSize_){
            break;
        }
    }
    count.join();
    return counts;
}

void AsyncMixin::countThread(std::array<unsigned long, 256>& counts){
    std::ranges::fill(counts, 0);
    while (true){
        std::vector<unsigned char> data = *readQueue_.wait_and_pop();
        for (unsigned char c : data){
            counts[c] += 1;
        }
        if (data.size() != chunkSize_){
            return;
        }
    }
}