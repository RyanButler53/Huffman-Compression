#ifdef HC_WITH_GPU
    #define NS_PRIVATE_IMPLEMENTATION
    #define MTL_PRIVATE_IMPLEMENTATION
#endif

#include "encoder.hpp"
#include <iostream>
#include <fstream>
#include <chrono>


using namespace std;

int main(int argc, const char** argv){
    if (argc == 1){
        cerr << "No files given" << endl;
        exit(1);
    }

    // Set compression on GPU if available
    execution::space space = execution::space::cpu;
    int arg_i = 1;
    std::string firstArg = argv[1];
    if (firstArg == "cpu"){
        space = execution::space::cpu;
        ++arg_i;
    } else if (firstArg == "gpu") {
        #ifdef HC_WITH_GPU
        space = execution::space::gpu;
        #endif
        ++arg_i;
    } else if (firstArg == "async"){
        space = execution::space::async;
        ++arg_i;
    } else if (firstArg == "async_gpu") {
        space = execution::space::async_gpu;
        ++arg_i;
    }
    for (; arg_i < argc; ++arg_i){
        auto now = std::chrono::steady_clock::now();
        string filename = argv[arg_i];
        std::unique_ptr<Encoder> e = Encoder::make(space, filename);
        e->Encode();
        auto done = std::chrono::steady_clock::now();

        long ms = std::chrono::duration_cast<std::chrono::milliseconds>((done - now)).count();
        std::cout << ms / 1000.0 << std::endl;
        auto [filelen, compSize, compRatio] = e->getStats();
        cout << "Original File Size: " << filelen << " bytes" << endl;
        cout << "Compressed File Size: " << compSize << " bytes" << endl;
        cout << "Compression Ratio: " << compRatio << endl;
    }

    return 0;
}
