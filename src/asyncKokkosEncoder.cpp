#ifdef HC_WITH_KOKKOS
#include "asyncKokkosEncoder.hpp"
#include <fstream>
#include <ranges>
#include <algorithm>
#include <array>
#include <thread>

void AsyncKokkosEncoder::init(){
    auto counts = asyncInit(filename_);
    buildFromFreq(counts);
}

void AsyncKokkosEncoder::readThread(std::array<std::string, 256>& codes){
    std::ifstream input{filename_, std::ios::in};
    if (!input.is_open()){
        std::cerr << "Unable to read file" << std::endl;
        writeQueue_.push({{}, true});
        return;
    }

    size_t maxBufferSize = std::ranges::max_element(codes, [](auto s1, auto s2){return s1.size() < s2.size();})->size();

    // Set up views and streams
    std::array<Kokkos::DefaultExecutionSpace, 2> streams{Kokkos::DefaultExecutionSpace(), Kokkos::DefaultExecutionSpace()};
    std::array<DeviceViewType, 2> deviceViews;
    std::array<DeviceViewType, 2> deviceOutViews;
    std::array<DeviceSubViewType, 2> deviceSubViews;
    std::array<DeviceSubViewType, 2> deviceSubOutViews;

    // Allocate buffers
    for (size_t i : std::views::iota(0,2)){
        deviceViews[i] = Kokkos::View<unsigned char*, Kokkos::DefaultExecutionSpace>(std::format("Device View in {}", i), maxBufferSize * chunkSize_);
        deviceOutViews[i] = Kokkos::View<unsigned char*, Kokkos::DefaultExecutionSpace>(std::format("Device Out View in {}", i), maxBufferSize * chunkSize_ / 8);
    }

    size_t buffer_i = 0;

    size_t numChunks = std::filesystem::file_size(filename_) / chunkSize_;

    // read into 2 512 KB chunks per iteration. 
    std::vector<unsigned char> filechunk(chunkSize_);
    std::string extraBytes;

    for (size_t step : std::views::iota(size_t{0}, numChunks)){
        // Reads 512 KB from file into file and convert to compressed string;
        auto readToCompressedString = [this, &codes, &extraBytes, &input, &filechunk](){
            std::string compressedString = extraBytes;
            // Read the first half of the chunk
            input.read((char*)filechunk.data(), chunkSize_ / 2);
            std::streamsize dataread = input.gcount();
    
            for (size_t i = 0; i < dataread; ++i){
                unsigned char c = filechunk[i];
                compressedString.append(codes[c]);
            }
            size_t extra = compressedString.size() % 8;
            std::copy(compressedString.end() - extra, compressedString.end(), extraBytes.begin());
            compressedString.erase(compressedString.length() - extra);
            return compressedString;
        };

        // Each iteration needs to have its own specially allocated host out views. 
        std::array<HostOutViewType, 2> hostOutViews;
        for (size_t buffer_i : std::views::iota(0, 2)){
            std::string compressedString = readToCompressedString();
            size_t n = compressedString.size();
            Kokkos::View<char*, Kokkos::HostSpace, Kokkos::MemoryUnmanaged> hostView(compressedString.data(), n);
    
            // Make Device View 0 that is buf[0] subviewed for the first compressedString.data() elements, resize device view and allocate host view
            deviceSubViews[buffer_i] = Kokkos::subview(deviceViews[buffer_i], Kokkos::make_pair<size_t, size_t>(0, n));
            deviceSubOutViews[buffer_i] = Kokkos::subview(deviceOutViews[buffer_i], Kokkos::make_pair<size_t, size_t>(0, n/8));
    
            // This must be allocated before any copying or work is launched on to the execution space
            hostOutViews[buffer_i] = Kokkos::create_mirror_view(deviceSubOutViews[buffer_i]);

            // Deep copy from host view to device view, launch kernel, copy back to host
            Kokkos::deep_copy(streams[buffer_i], deviceSubViews[buffer_i], hostView);
            Kokkos::RangePolicy<Kokkos::DefaultExecutionSpace> rPolicy(streams[buffer_i], 0, n/8);
            // auto kernel = getKernel(0);
            Kokkos::parallel_for("Compress Buffer 0", rPolicy, KOKKOS_LAMBDA(const uint32_t i){
                unsigned char ch = 0;
                for (size_t b = 0; b < 8; ++b){
                    ch = (ch << 1) | (deviceSubViews[buffer_i][8*i + b] == 49); // 49 is ascii for "1"
                }
                deviceSubOutViews[buffer_i](i) = ch;
            });
            Kokkos::deep_copy(streams[buffer_i], hostOutViews[buffer_i], deviceSubOutViews[buffer_i]);
        }
        
        // Wait for devices space to be copied back to the host. 
        streams[0].fence();
        writeQueue_.push({hostOutViews[0], false});
        streams[1].fence();
        writeQueue_.push({hostOutViews[1], false});

    }

    // Do special logic for the LAST chunk
    std::string compressedString = extraBytes;
    // Read the first half of the chunk
    input.read((char*)filechunk.data(), chunkSize_ / 2);
    std::streamsize dataread = input.gcount();

    for (size_t i = 0; i < dataread; ++i){
        unsigned char c = filechunk[i];
        compressedString.append(codes[c]);
    }
    while (compressedString.length() % 8 != 0){
        compressedString.append("1");
    }

    // Send last chunk to GPU. Use buffer 0. 
    size_t n = compressedString.size();
    Kokkos::View<char*, Kokkos::HostSpace, Kokkos::MemoryUnmanaged> hostView(compressedString.data(), n);

    // Make Device View 0 that is buf[0] subviewed for the first compressedString.data() elements, resize device view and allocate host view
    deviceSubViews[0] = Kokkos::subview(deviceViews[0], Kokkos::make_pair<size_t, size_t>(0, n));
    deviceSubOutViews[0] = Kokkos::subview(deviceOutViews[0], Kokkos::make_pair<size_t, size_t>(0, n/8));

    // This must be allocated before any copying or work is launched on to the execution space
    typename DeviceSubViewType::host_mirror_type hostOutView0 = Kokkos::create_mirror_view(deviceSubOutViews[0]);
    
    // Deep copy from host view to device view, launch kernel, copy back to host
    Kokkos::deep_copy(streams[0], deviceSubViews[0], hostView);
    Kokkos::RangePolicy<Kokkos::DefaultExecutionSpace> policy0(streams[0], 0, n/8);
    // auto kernel = getKernel(0);
    Kokkos::parallel_for("Compress Buffer 0", policy0, KOKKOS_LAMBDA(const uint32_t i){
        unsigned char ch = 0;
        for (size_t b = 0; b < 8; ++b){
            ch = (ch << 1) | (deviceSubViews[0][8*i + b] == 49); // 49 is ascii for "1"
        }
        deviceSubOutViews[0](i) = ch;
    });
    Kokkos::deep_copy(streams[0],hostOutView0, deviceSubOutViews[0]);
    streams[0].fence();
    writeQueue_.push({hostOutView0, true});
}


void AsyncKokkosEncoder::writeThread(){
    bool last = false;
    std::ofstream out{filename_ + ".compress", std::ios::out};
    while (!last){
        auto [buffer, lastMsg] = *writeQueue_.wait_and_pop();
        last = lastMsg;
        if(buffer.span_is_contiguous()){
            out.write((char*) buffer.data(), buffer.size());
        } else {
            std::cout << "Error! View is not contiguous!" << std::endl;
        }
    }
}

void AsyncKokkosEncoder::writeToFile(std::array<std::string, 256>& codes){
    
    std::thread write([this](){writeThread();});
    readThread(codes);
 
    write.join();
}

#endif