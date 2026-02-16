#ifdef HC_WITH_GPU
#include "metalEncoder.hpp"

GpuManager::GpuManager(MTL::Device* device, size_t stringLength):
device_{device}, stringLen_{stringLength}
{
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

GpuManager::~GpuManager()
{
    pipeline_->release();
    commandQueue_->release();
}

// Setting up the kernel
void GpuManager::encodeCommand(MTL::ComputeCommandEncoder* computeEncoder, 
                                 MTL::Buffer* compressedString, MTL::Buffer* compressedBytes){

    computeEncoder->setComputePipelineState(pipeline_);
    computeEncoder->setBuffer(compressedString, 0, 0);
    computeEncoder->setBuffer(compressedBytes, 0, 1);

    // Bytes buffer is string buffer / 8. 
    size_t arraySize = stringLen_ / 8;
    MTL::Size gridSize = MTL::Size::Make(arraySize, 1, 1);
    NS::UInteger threadGroupSize = pipeline_->maxTotalThreadsPerThreadgroup();
    if (threadGroupSize > arraySize) {
        threadGroupSize = arraySize;
    }
    MTL::Size threadgroupSize = MTL::Size::Make(threadGroupSize, 1, 1);
    // Encode the compute command.
    computeEncoder->dispatchThreads(gridSize, threadgroupSize);

}

void  GpuManager::compress(MTL::Buffer* compressedString, MTL::Buffer* compressedBytes){
    MTL::CommandBuffer* commandBuffer = commandQueue_->commandBuffer();
    MTL::ComputeCommandEncoder* computeEncoder = commandBuffer->computeCommandEncoder();

    encodeCommand(computeEncoder, compressedString, compressedBytes);

    // End the compute pass.
    computeEncoder->endEncoding();
    // Execute the command.
    commandBuffer->commit();

    commandBuffer->waitUntilCompleted();
}

void MetalEncoder::getCompressedBytes(std::vector<unsigned char>& compressedChars, std::string& compressedString) {
    compressedChars.resize(compressedString.size() / 8);

    MTL::Device* d = MTL::CreateSystemDefaultDevice();
    size_t nbytes = compressedString.size();
    
    // not technically device memory...
    MTL::Buffer* d_compString = d->newBuffer(nbytes, MTL::ResourceStorageModeShared);
    MTL::Buffer* d_compBytes = d->newBuffer(nbytes / 8, MTL::ResourceStorageModeShared);
    std::copy(compressedString.begin(), compressedString.end(), (char*)d_compString->contents());

    GpuManager gpu(d, nbytes);

    gpu.compress(d_compString, d_compBytes);
    std::copy((char*)d_compBytes->contents(), (char*)d_compBytes->contents() + d_compBytes->length(), compressedChars.begin());
    d->release();
}

#endif