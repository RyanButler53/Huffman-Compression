#pragma once

#ifdef HC_WITH_GPU

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

#include "encoder.hpp"

class GpuManager
{
    MTL::Device* device_;
    MTL::ComputePipelineState* pipeline_;
    MTL::CommandQueue* commandQueue_; // stream
    size_t stringLen_; // length of the string as string characters, not bytes;

    void encodeCommand(MTL::ComputeCommandEncoder *computeEncoder, MTL::Buffer* compressedString, MTL::Buffer* compressedBytes);
    
    public:
    GpuManager(MTL::Device* device, size_t stringLength);
    ~GpuManager();

    void compress(MTL::Buffer* compressedString, MTL::Buffer* compressedBytes);
};

class MetalEncoder : public Encoder {
    
    void getCompressedBytes(std::vector<unsigned char>& compressedChars, std::string& compressedString) override;

    public: 
    using Encoder::Encoder;

};

#endif