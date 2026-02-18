#pragma once

#ifdef HC_WITH_GPU


#include "gpuMixin.hpp"
#include "encoder.hpp"

class MetalEncoder : public Encoder, GpuMixin {
    
    void compress(MTL::Buffer* compressedString, MTL::Buffer* compressedBytes);
    void getCompressedBytes(std::vector<unsigned char>& compressedChars, std::string& compressedString) override;

    public: 
    using Encoder::Encoder;
    using GpuMixin::GpuMixin;

};

#endif