add_library(node STATIC huffmanNode.cpp)
target_include_directories(node PUBLIC ${CMAKE_SOURCE_DIR}/inc)

set(ENCODE_SRC encoder.cpp AsyncEncoder.cpp $<IF:$<BOOL:${HC_WITH_GPU}>,metalEncoder.cpp,"">)
add_library(encode STATIC ${ENCODE_SRC} AsyncEncoder.cpp)
target_link_libraries(encode PUBLIC node $<IF:$<BOOL:${HC_WITH_GPU}>,metal-cpp,"">)
target_compile_definitions(encode PUBLIC $<IF:$<BOOL:${HC_WITH_GPU}>,"HC_WITH_GPU","">)

add_library(decode STATIC decoder.cpp)
target_link_libraries(decode PUBLIC node)


# Driver executables
add_executable(compress compress.cpp)
target_link_libraries(compress PUBLIC encode)

add_executable(uncompress uncompress.cpp)
target_link_libraries(uncompress PRIVATE decode)