A C++ Implementation of the Huffman Compression algorithm. 

### Build: 

`make`

### Compress:

`./compress <filename>`

### Uncompress:

`./uncompress <filename>`

### Clear compressed files, codes and uncompressed files:

`make clearcomp`

### Notes: 

The `compress` command will make the file `filename.compress` and `filename.compress.codes`. To uncompress, just do `uncompress filename`. The `.compress` and `.codes` are already handled. 

### Roadmap: 

- Support for multiple files.
- Test script
- compressing folders
