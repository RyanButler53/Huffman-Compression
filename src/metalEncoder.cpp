#ifdef HC_WITH_GPU
#include "metalEncoder.hpp"


void MetalEncoder::compress(MTL::Buffer* compressedString, MTL::Buffer* compressedBytes){
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

    size_t nbytes = compressedString.size();
    
    // not technically device memory...
    MTL::Buffer* d_compString = device_->newBuffer(nbytes, MTL::ResourceStorageModeShared);
    MTL::Buffer* d_compBytes = device_->newBuffer(nbytes / 8, MTL::ResourceStorageModeShared);
    std::copy(compressedString.begin(), compressedString.end(), (char*)d_compString->contents());


    compress(d_compString, d_compBytes);
    std::copy((char*)d_compBytes->contents(), (char*)d_compBytes->contents() + d_compBytes->length(), compressedChars.begin());
}

#endif