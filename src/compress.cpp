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
        auto now = std::chrono::steady_clock::now();
        string filename = argv[i];
        Encoder e{filename};
        e.Encode();
        auto done = std::chrono::steady_clock::now();

        long ms = std::chrono::duration_cast<std::chrono::milliseconds>((done - now)).count();
        std::cout << ms / 1000.0 << std::endl;
    }

    return 0;
}
