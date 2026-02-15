#include <metal_stdlib>


kernel void compress_bytes(device const char* compressedString,
                           device char* compressedBytes,
                           uint i [[thread_position_in_grid]]){

    char ch = 0;
    for (int b = 0; b < 8; ++b){
        ch = (ch << 1) | (compressedString[8*i + b] == 49);
    }
    compressedBytes[i] = ch;
}