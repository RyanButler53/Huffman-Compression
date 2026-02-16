#pragma once

#ifdef HC_WITH_GPU

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

class MetalEncoder
{
    MTL::Device* device_;
    MTL::ComputePipelineState* pipeline_;
    MTL::CommandQueue* commandQueue_; // stream
    MTL::Library* library_;
    size_t stringLen_; // length of the string as string characters, not bytes;

    void encodeCommand(MTL::ComputeCommandEncoder *computeEncoder, MTL::Buffer* compressedString, MTL::Buffer* compressedBytes);


public:
    MetalEncoder(MTL::Device* device, size_t stringLength);
    ~MetalEncoder();

    void compress(MTL::Buffer* compressedString, MTL::Buffer* compressedBytes);
};

#endif