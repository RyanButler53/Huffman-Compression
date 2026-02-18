#ifdef HC_WITH_GPU
#include "asyncGpuEncoder.hpp"
#include <fstream>
#include <thread>
#include <iostream>


void AsyncGpuEncoder::readThread(std::array<std::string, 256>& codes){
    std::fstream input{filename_, std::ios::in};
    unsigned char c;
    if (!input.is_open()){
        std::cerr << "Unable to read file" << std::endl;
        // compressQueue_.push({"", true});
    }
    // read into 1 MB chunks
    const size_t chunkSize = 1024 * 1024;
    std::vector<unsigned char> filechunk(chunkSize);
    std::string extraBytes;
    while (true){
        std::string compressedString = extraBytes;
        input.read((char*)filechunk.data(), chunkSize);
        std::streamsize dataread = input.gcount();

        for (size_t i = 0; i < dataread; ++i){
            unsigned char c = filechunk[i];
            compressedString.append(codes[c]);
        }
        // Copy the compressed string into a metal buffer
        // First pass, have constant buffer size to copy DIRECTLY to the buffer
        
        if (dataread < chunkSize){ // last one
            // Pad the rest of the values with ones. 
            while (compressedString.length() % 8 != 0){
                compressedString.append("1");
            }
            pushToGpu(compressedString, true);
            break;
        } else {
            size_t extra = compressedString.size() % 8;
            std::copy(compressedString.end() - extra, compressedString.end(), extraBytes.begin());
            compressedString.erase(compressedString.length() - extra);
            pushToGpu(compressedString, false); 
        }
    }
}

void AsyncGpuEncoder::pushToGpu(std::string& compressedString, bool last){
    if (compressedString.empty()){
        writeQueue_.push({nullptr, 0, true});
    }
    // Make Buffers
    size_t nbytes = compressedString.size();
    MTL::Buffer* d_compString = device_->newBuffer(nbytes, MTL::ResourceStorageModeShared);
    MTL::Buffer* d_compBytes = device_->newBuffer(nbytes / 8, MTL::ResourceStorageModeShared);
    
    // Set up kernel call
    MTL::CommandBuffer* commandBuffer = commandQueue_->commandBuffer();
    MTL::ComputeCommandEncoder* computeEncoder = commandBuffer->computeCommandEncoder();
    encodeCommand(computeEncoder, d_compString, d_compBytes);
    computeEncoder->endEncoding();

    commandBuffer->addCompletedHandler([this, d_compBytes, last](MTL::CommandBuffer* commandBuffer){
        writeQueue_.push({d_compBytes, d_compBytes->length(), last});
    });

    commandBuffer->commit();
}

void AsyncGpuEncoder::writeThread() {
    bool last = false;
    std::ofstream out{filename_ + ".compress", std::ios::out};
    while (!last){
        auto [buffer, n, lastMsg] = *writeQueue_.wait_and_pop();
        last = lastMsg;
        if (!buffer){
            break;
        }
        out.write((char*) buffer->contents(), n);
        buffer->release();
    }
}

void AsyncGpuEncoder::writeToFile(std::array<std::string, 256>& codes){
    std::thread read([this, &codes](){readThread(codes);});
    std::thread write([this](){writeThread();});

    read.join();
    write.join();
}

#endif
