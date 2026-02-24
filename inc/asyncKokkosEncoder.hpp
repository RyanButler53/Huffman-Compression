#pragma once

#ifdef HC_WITH_KOKKOS

#include <Kokkos_Core.hpp>
#include "encoder.hpp"
#include <asyncMixin.hpp>
#include <array>



class AsyncKokkosEncoder : public Encoder, AsyncMixin {

    void init() override;

    public:
    using Encoder::Encoder;

    // void writeToFile(std::array<std::string, 256>& codes) override;
    void getCompressedBytes(std::vector<unsigned char>& compressedChars, std::string& compressedString) override;

};
#endif

// Walk: Load the entire compressed string into memory, compress it at once, read it out at once
// Run: Load a chunk of compressed string into memory, asynchronously compress and write it out. 