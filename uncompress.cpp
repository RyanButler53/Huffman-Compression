#include "huffmanNode.hpp"
#include "decoder.hpp"
#include <iostream>

using namespace std;

int main(int argc, const char** argv){
    std::string filename = "small.txt";
    if (argc > 1){
        filename = argv[1];
    }
    Decoder d{filename};
    d.Decode();
}
