#pragma once
#include "threadsafeQueue.hpp"
#include "encoder.hpp"
#include "asyncMixin.hpp"
#include <string>
#include <vector>
#include <tuple>
#include <array>

class AsyncEncoder : public Encoder, AsyncMixin {

    ThreadsafeQueue<std::pair<std::string, bool>> compressQueue_;
    ThreadsafeQueue<std::pair<std::vector<unsigned char>, bool>> writeQueue_;

    // Main Encoding Asynchronous operations
    void readThread(std::array<std::string, 256>& codes); // returns the total file size
    void compressThread(); // returns the compressed file size
    void writeThread();

    // Override initialization for async initialization
    void init() override;

    public:

    using Encoder::Encoder;
    void writeToFile(std::array<std::string, 256>& codes) override;
};
      