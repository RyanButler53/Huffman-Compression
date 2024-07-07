A C++ Implementation of the Huffman Compression algorithm. 

### Build: 

`make`

### Compress:

`./compress <filename1> <filename2> ...`

### Uncompress:

`./uncompress <filename1> <filename2> ...`

### Test Script: 

`sh test-script.sh`

### Clear compressed files, codes and uncompressed files:

`sh clearCompress.sh`

### Notes: 

The `compress` command will make the file `filename.compress` and `filename.compress.codes`. To uncompress, just do `uncompress filename`. The `.compress` and `.codes` are already handled. 

### Roadmap: 

- compressing folders
