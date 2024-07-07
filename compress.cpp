#include "huffmanNode.hpp"
#include "encoder.hpp"
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, const char** argv){
    string filename = "small.txt";
    if (argc > 1){
        filename = argv[1];
    }
    Encoder e{filename};
    e.Encode();

    return 0;
}
