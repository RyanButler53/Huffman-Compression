#pragma once
#include "threadsafeQueue.hpp"
#include <string>
#include <vector>
#include <tuple>
#include <array>

class AsyncEncoder {

    ThreadsafeQueue<std::pair<std::string, bool>> compressQueue_;
    ThreadsafeQueue<std::pair<std::vector<unsigned char>, bool>> writeQueue_;
    const std::array<std::string, 256>& codes_;
    std::string filename_;


    size_t readThread(); // returns the total file size
    size_t compressThread(); // returns the compressed file size
    void writeThread();

    public:

    AsyncEncoder(const std::array<std::string, 256>& codes, std::string filename);
    std::pair<size_t, size_t> launch(); 
    };
      