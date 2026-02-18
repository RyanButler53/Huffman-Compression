#ifdef HC_WITH_GPU

#include "gpuMixin.hpp"

GpuMixin::GpuMixin(){
    device_ = MTL::CreateSystemDefaultDevice();
    if (!device_){
        throw("Error opening device");
    }

    NS::Error *error = nullptr;
    MTL::Library* library = device_->newDefaultLibrary();
    if (!library){throw std::invalid_argument("Could not load library");}

    commandQueue_ = device_->newCommandQueue();
    if (!commandQueue_){ throw std::invalid_argument("Error initializing command queue"); }

    auto compressStr = NS::String::string("compress_bytes", NS::ASCIIStringEncoding);
    MTL::Function* compressFunc = library->newFunction(compressStr);

    pipeline_ = device_->newComputePipelineState(compressFunc, &error);
    compressFunc->release();
    library->release();
}

GpuMixin::~GpuMixin(){
    device_->release();
    commandQueue_->release();
    pipeline_->release();
}

void GpuMixin::encodeCommand(MTL::ComputeCommandEncoder* computeEncoder, 
    MTL::Buffer* compressedString, MTL::Buffer* compressedBytes){

    computeEncoder->setComputePipelineState(pipeline_);
    computeEncoder->setBuffer(compressedString, 0, 0);
    computeEncoder->setBuffer(compressedBytes, 0, 1);

    // Bytes buffer is string buffer / 8. 
    size_t arraySize = compressedString->length() / 8;
    MTL::Size gridSize = MTL::Size::Make(arraySize, 1, 1);
    NS::UInteger threadGroupSize = pipeline_->maxTotalThreadsPerThreadgroup();
    if (threadGroupSize > arraySize) {
        threadGroupSize = arraySize;
    }
    MTL::Size threadgroupSize = MTL::Size::Make(threadGroupSize, 1, 1);
    // Encode the compute command.
    computeEncoder->dispatchThreads(gridSize, threadgroupSize);
}

#endif
