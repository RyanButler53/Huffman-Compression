add_library(node STATIC huffmanNode.cpp)
target_include_directories(node PUBLIC ${CMAKE_SOURCE_DIR}/inc)

add_library(encode STATIC encoder.cpp)
target_link_libraries(encode PUBLIC node)

add_library(decode STATIC decoder.cpp)
target_link_libraries(decode PUBLIC node)


add_executable(compress compress.cpp)
target_link_libraries(compress PRIVATE encode)

add_executable(uncompress uncompress.cpp)
target_link_libraries(uncompress PRIVATE decode)