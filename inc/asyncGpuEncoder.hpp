#ifdef HC_WITH_GPU
#pragma once


#include "gpuEncoder.hpp"

class AsyncGpuEncoder : public GpuEncoder{

    struct CompressedBytesPacket
    {
        MTL::Buffer* data;
        size_t length;
        bool last;
    };
    

    ThreadsafeQueue<CompressedBytesPacket> writeQueue_;


    // Async Functions
    void readThread(std::array<std::string, 256>& codes);
    void writeThread();
    void pushToGpu(std::string& compressedString, bool last);


    public:
    // Open GPU and set up kernels for async
    using GpuEncoder::GpuEncoder;

    void writeToFile(std::array<std::string, 256>& codes) override;
};

#endif