#ifdef HC_WITH_GPU
#pragma once
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <string>

#include "encoder.hpp"

class GpuMixin {

    protected:

    // GPU Variables
    MTL::Device* device_;
    MTL::CommandQueue* commandQueue_;
    MTL::ComputePipelineState* pipeline_;

    public:
    GpuMixin();
    virtual ~GpuMixin();

    void encodeCommand(MTL::ComputeCommandEncoder *computeEncoder, MTL::Buffer* compressedString, MTL::Buffer* compressedBytes);
};

#endif  // HC_WITH_GPU