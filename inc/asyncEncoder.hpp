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

    void readThread(std::array<std::string, 256>& codes); // returns the total file size
    void compressThread(); // returns the compressed file size
    void writeThread();

    public:

    using Encoder::Encoder;
    void writeToFile(std::array<std::string, 256>& codes) override;
};
      