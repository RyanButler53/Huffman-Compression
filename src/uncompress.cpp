#include "huffmanNode.hpp"
#include "decoder.hpp"
#include <iostream>

using namespace std;

int main(int argc, const char** argv){
    if (argc == 1){
        cerr << "No files given" << endl;
        exit(1);
    }

    for (int i = 1; i < argc; ++i) {
        string filename = argv[i];
        Decoder d{filename};
        d.Decode();
    }
    return 0;
}
