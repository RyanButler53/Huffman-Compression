include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723 
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

find_package(OpenSSL REQUIRED)

add_executable(equivalence equivalent.cpp)
message(STATUS "Current source: ${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(equivalence PRIVATE gtest gtest_main encode decode OpenSSL::Crypto)
target_compile_definitions(equivalence PUBLIC TEST_FILE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
