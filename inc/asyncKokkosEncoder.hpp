#pragma once

#ifdef HC_WITH_KOKKOS

#include <Kokkos_Core.hpp>
#include "encoder.hpp"
#include <asyncMixin.hpp>
#include <array>



class AsyncKokkosEncoder : public Encoder, AsyncMixin {

    void init() override;
    ThreadsafeQueue<std::pair<std::string, bool>> compressQueue_;
    ThreadsafeQueue<std::pair<std::vector<unsigned char>, bool>> writeQueue_;
    void writeThread();
    void compressThread();
    void readThread(std::array<std::string, 256>& codes);

    public:
    using Encoder::Encoder;

    void writeToFile(std::array<std::string, 256>& codes) override;

};
#endif

// Walk: Load the entire compressed string into memory, compress it at once, read it out at once
// Run: Load a chunk of compressed string into memory, asynchronously compress and write it out. 
// Sprint: Use 2 buffers to pipeline it