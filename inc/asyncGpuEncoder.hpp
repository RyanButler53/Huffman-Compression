#ifdef HC_WITH_GPU
#pragma once

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

#include <string>

#include "encoder.hpp"

class AsyncGpuEncoder : public Encoder{

    struct CompressedBytesPacket
    {
        MTL::Buffer* data;
        size_t length;
        bool last;
    };
    

    ThreadsafeQueue<CompressedBytesPacket> writeQueue_;

    // GPU Variables
    MTL::Device* device_;
    MTL::CommandQueue* commandQueue_;
    MTL::ComputePipelineState* pipeline_;

    void encodeCommand(MTL::ComputeCommandEncoder* computeEncoder, 
        MTL::Buffer* compressedString, MTL::Buffer* compressedBytes);

    // Async Functions
    void readThread(std::array<std::string, 256>& codes);
    void writeThread();
    void pushToGpu(std::string& compressedString, bool last);


    public:
    // Open GPU and set up kernels for async
    AsyncGpuEncoder(std::string file);

    void writeToFile(std::array<std::string, 256>& codes) override;
};

#endif