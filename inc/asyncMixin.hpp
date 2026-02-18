
#pragma once
#include <threadsafeQueue.hpp>
#include <vector>
#include <string>

class AsyncMixin
{
private:

    ThreadsafeQueue<std::vector<unsigned char>> readQueue_;
    void countThread(std::array<unsigned long, 256>& codes);

protected:    

    const size_t chunkSize_ = 1024 * 1024;
    AsyncMixin() = default;
    virtual ~AsyncMixin() {}

    std::array<unsigned long, 256> asyncInit(std::string filename);
};

