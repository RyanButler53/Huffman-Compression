#pragma once
#include "threadsafeQueue.hpp"
#include "encoder.hpp"
#include <string>
#include <vector>
#include <tuple>
#include <array>

class AsyncEncoder : public Encoder {

    ThreadsafeQueue<std::pair<std::string, bool>> compressQueue_;
    ThreadsafeQueue<std::pair<std::vector<unsigned char>, bool>> writeQueue_;
    ThreadsafeQueue<std::vector<unsigned char>> readQueue_;

    const size_t chunkSize_ = 1024 * 1024;

    void readThread(std::array<std::string, 256>& codes); // returns the total file size
    void compressThread(); // returns the compressed file size
    void writeThread();
    void countThread(std::array<unsigned long, 256>& codes);


    void init() override;

    public:

    using Encoder::Encoder;
    void writeToFile(std::array<std::string, 256>& codes) override;
};
      