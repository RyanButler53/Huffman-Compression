A C++/Metal Implementation of the Huffman Compression algorithm. 

### Build: 

`mkdir build`
`cd build`
`cmake ..`
`cmake --build . --parallel`

### Build Options: 
- `-DHC_DO_TESTS=ON`:  Turns on tests. See the Tests section
- `-DHC_WITH_GPU=ON`: Tries to build with Metal-cpp bindings. Must provide `-Dmetal_ROOT=/path/to/metal-cpp/directory`. Apple only. Also requires xcode to be downloaded to run the `xcrun -sdk macosx metal` command to compile the kernels

### Compress:

`./compress [cpu|gpu|async|async_gpu] <filename1> <filename2> ...`

### Uncompress:

`./uncompress <filename1> <filename2> ...`

### Tests: 

Building tests requires gtest and openssl. Gtest is downloaded automatically through fetchContent and openssl is found via cmake find package. 
To turn off tests , use `-DHC_DO_TESTS=OFF` in the cmake configuration. 

### Clear compressed files, codes and uncompressed files:

`sh clearCompress.sh`

### Notes: 

The `compress` command will make the file `filename.compress` and `filename.compress.codes`. To uncompress, just do `uncompress filename`. The `.compress` and `.codes` are already handled. 

### Roadmap: 

- Asynchronous compression using kokkos or metal. 
