#include "huffmanNode.hpp"
#include "encoder.hpp"
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, const char** argv){
    if (argc == 1){
        cerr << "No files given" << endl;
        exit(1);
    }

    for (int i = 1; i < argc; ++i){
        string filename = argv[i];
        Encoder e{filename};
        e.Encode();
    }
    return 0;
}
