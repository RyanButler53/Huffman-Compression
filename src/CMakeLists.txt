add_library(node STATIC huffmanNode.cpp)
target_include_directories(node PUBLIC ${CMAKE_SOURCE_DIR}/inc)

set(ENCODE_KOKKOS_SRC asyncKokkosEncoder.cpp)
set(ENCODE_GPU_SRC metalEncoder.cpp AsyncGpuEncoder.cpp gpuMixin.cpp)
set(ENCODE_SRC encoder.cpp AsyncEncoder.cpp asyncMixin.cpp 
               $<$<BOOL:${HC_WITH_GPU}>:${ENCODE_GPU_SRC}> 
               $<$<BOOL:${HC_WITH_KOKKOS}>:${ENCODE_KOKKOS_SRC}> )

add_library(encode STATIC ${ENCODE_SRC})
target_link_libraries(encode PUBLIC node $<$<BOOL:${HC_WITH_GPU}>:metal-cpp> $<$<BOOL:${HC_WITH_KOKKOS}>:Kokkos::kokkos>)
target_compile_definitions(encode PUBLIC $<$<BOOL:${HC_WITH_GPU}>:"HC_WITH_GPU"> $<$<BOOL:${HC_WITH_KOKKOS}>:"HC_WITH_KOKKOS">)

add_library(decode STATIC decoder.cpp)
target_link_libraries(decode PUBLIC node)


# Driver executables
add_executable(compress compress.cpp)
target_link_libraries(compress PUBLIC encode)

add_executable(uncompress uncompress.cpp)
target_link_libraries(uncompress PRIVATE decode)