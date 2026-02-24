#ifdef HC_WITH_KOKKOS
#include "asyncKokkosEncoder.hpp"


void AsyncKokkosEncoder::init(){
    auto counts = asyncInit(filename_);
    buildFromFreq(counts);
}

// void AsyncKokkosEncoder::writeToFile(std::array<std::string, 256>& codes){

// }

// Memory Spaces: DefaultHostExecutionSpace for host, DefaultExecutionSpace for "device"

void AsyncKokkosEncoder::getCompressedBytes(std::vector<unsigned char>& compressedChars, std::string& compressedString){
    size_t nbytes = compressedString.size();
    compressedChars.resize(nbytes / 8);

    // Copy the compressedString to a Kokkos View in execution space
    Kokkos::View<char*, Kokkos::HostSpace, Kokkos::MemoryUnmanaged> compStringView(compressedString.data(), nbytes);
    Kokkos::View<char*, Kokkos::DefaultExecutionSpace> d_compString("Device Compressed String", nbytes);
    Kokkos::View<unsigned char*, Kokkos::DefaultExecutionSpace> d_compBytes("Device Bytes", nbytes / 8);

    Kokkos::deep_copy(d_compString, compStringView);

    // Compress down to bytes
    Kokkos::parallel_for("Convert to bytes", nbytes / 8, KOKKOS_LAMBDA(const uint32_t i){
        unsigned char ch = 0;
        for (size_t b = 0; b < 8; ++b){
            ch = (ch << 1) | (d_compString[8*i + b] == 49); // 49 is ascii for "1"
        }
        d_compBytes(i) = ch;
    });
  

    // Let all the threads finish executing
    Kokkos::fence();

    Kokkos::View<unsigned char*, Kokkos::HostSpace, Kokkos::MemoryTraits<Kokkos::Unmanaged>> bytesView(compressedChars.data(), nbytes/8);
    // Copy it back to the host

    Kokkos::deep_copy(bytesView, d_compBytes);
}

#endif