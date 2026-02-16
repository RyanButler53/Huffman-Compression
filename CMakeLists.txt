cmake_minimum_required(VERSION 4.0)
project(Huffman VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(HC_DO_TESTS "build unit tests" ON)
option(HC_WITH_GPU "build with metal support" ON)

add_subdirectory(src)
set (metal_ROOT /users/ryan/projects/sw/metal-cpp)
if (HC_WITH_GPU)
    message(STATUS "Enabling GPU support via Metal")
    add_subdirectory(kernels)
    find_package(Metal)
endif()

if (HC_DO_TESTS)
    add_subdirectory(tests)
endif()