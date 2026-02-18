#pragma once

#ifdef HC_WITH_GPU


#include "gpuEncoder.hpp"



class MetalEncoder : public GpuEncoder {
    
    void compress(MTL::Buffer* compressedString, MTL::Buffer* compressedBytes);
    void getCompressedBytes(std::vector<unsigned char>& compressedChars, std::string& compressedString) override;

    public: 
    using GpuEncoder::GpuEncoder;

};

#endif