A C++ Implementation of the Huffman Compression algorithm. 

### Build: 

`mkdir build`
`cd build`
`cmake ..`
`cmake --build . --parallel`

### Compress:

`./compress <filename1> <filename2> ...`

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

- compressing folders
