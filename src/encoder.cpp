#include "encoder.hpp"
#include <queue>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <ranges>

#ifdef HC_WITH_GPU
#include "metalEncoder.hpp"
#endif

using namespace std;

Encoder::Encoder(std::string file):
filename_{file}, huffmanTree_{nullptr}, fileLen_{0}, compFileLen_{0}
{
    // Read in from file, build a huffman tree of frequencies. 
    fstream input{filename_};
    std::array<unsigned long, 256> counts;
    std::ranges::fill(counts, 0);
    unsigned char c;
    fileLen_ = 0;
    if (input.is_open()) {
        while (input >> noskipws >> c){
            ++counts[c];
            ++fileLen_;
        }
    } else {
        cerr << "Couldn't open file" << endl;
        exit(1);
    }
    buildFromFreq(counts);
}

Encoder::~Encoder()
{
    destructorHelper(huffmanTree_);
}

void Encoder::destructorHelper(HuffmanNode *&node){
    if (node != nullptr){
        if (node->right_){
            destructorHelper(node->right_);
        }
        if (node->left_){
            destructorHelper(node->left_);
        }
        delete node;
    }
}

void Encoder::codesHelper(const string& currentCode, HuffmanNode* curNode, std::array<std::string, 256>& codes){
    if (curNode->isLeaf()){
        unsigned char symbol = curNode->symbol_;
        codes[symbol] = currentCode;
    } else {
        codesHelper(currentCode + "0", curNode->left_, codes);
        codesHelper(currentCode + "1", curNode->right_, codes);
    }
}

void Encoder::buildFromFreq(std::array<unsigned long, 256> freqs){
    priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, HuffmanPtrCompare> q;
    size_t n = freqs.size() - 1; // number of internal nodes
    for (size_t c = 0; c < 256; ++c){
        unsigned long freq = freqs[c];
        q.push(new HuffmanNode(c, freq));
    }

    // Build the huffman Tree.
    for (size_t i = 0; i < n; ++i){
        HuffmanNode* left = q.top();
        q.pop();
        HuffmanNode* right = q.top();
        q.pop();
        HuffmanNode *newNode = new HuffmanNode{left, right, left->freq_ + right->freq_};
        q.push(newNode);
    }
    huffmanTree_ = q.top();
    return;
}

std::array<std::string, 256> Encoder::getCodes(){
    std::array<std::string, 256> codes;
    std::ranges::fill(codes, "");
    codesHelper("", huffmanTree_, codes);
    return codes;
}

void Encoder::getCompressedString(std::string& compressedString, std::array<std::string, 256>& codes ){
    fstream input{filename_};
    unsigned char c;
    if (!input.is_open()){
        cerr << "Unable to read file" << endl;
        return;
    }
    
    while (input >> noskipws >> c){
        compressedString.append(codes[c]);
    }

    // Pad the rest of the values with ones. 
    while (compressedString.length() % 8 != 0){
        compressedString.append("1");
    }
}

void Encoder::getCompressedBytes(execution::space space, std::vector<unsigned char>& compressedChars, std::string& compressedString){

    if (space == execution::space::cpu){
        compressedChars.reserve(compressedString.size() / 8);

        string::iterator it = begin(compressedString);
        while(it != end(compressedString)){
            unsigned char ch = 0;
            for (size_t i = 0; i < 8; ++i){
                ch = (ch << 1) | (*it == 49); // 49 is ascii for "1"
                ++it;
            }
            compressedChars.push_back(ch);
        }
        return;
    } else { // gpu
     #ifdef HC_WITH_GPU
        compressedChars.resize(compressedString.size() / 8);

        MTL::Device* d = MTL::CreateSystemDefaultDevice();
        size_t nbytes = compressedString.size();
        
        // not technically device memory...
        MTL::Buffer* d_compString = d->newBuffer(nbytes, MTL::ResourceStorageModeShared);
        MTL::Buffer* d_compBytes = d->newBuffer(nbytes / 8, MTL::ResourceStorageModeShared);
        std::copy(compressedString.begin(), compressedString.end(), (char*)d_compString->contents());

        MetalEncoder encoder(d, nbytes);

        encoder.compress(d_compString, d_compBytes);
        std::copy((char*)d_compBytes->contents(), (char*)d_compBytes->contents() + d_compBytes->length(), compressedChars.begin());
        d->release();
     #else
        throw std::logic_error("Compression not compiled with GPU support");
     #endif
    }

}

void Encoder::writeToFile(std::vector<unsigned char>& compressedChars){
    // Print to file
    ofstream out{filename_ + ".compress"};
    for (unsigned char& c : compressedChars){
        out << c;
    }
}

void Encoder::writeToFile(std::array<std::string, 256>& codes){
    // Have to read in the REAL FILE DATA
    string compressedString;
    getCompressedString(compressedString, codes);
    
    // turn into bytes.
    vector<unsigned char> compressedChars;
    getCompressedBytes(execution::space::gpu, compressedChars, compressedString);
    compFileLen_ = compressedChars.size();

    writeToFile(compressedChars);
}

void Encoder::writeCodes(std::array<std::string, 256> &codes){
    ofstream out{filename_ + ".compress.codes"};
    out << codes.size() << endl;
    out << fileLen_ << endl;
    for (size_t i = 0; i < 256; ++i){
        if (codes[i] != ""){
            out << i << " " << codes[i] << endl;
        }
    }
}

void Encoder::Encode(){
    std::array<std::string, 256>codes = getCodes();
    writeToFile(codes);
    writeCodes(codes);

    cout << "Original File Size: " << fileLen_ << " bytes" << endl;
    cout << "Compressed File Size: " << compFileLen_ << " bytes" << endl;
    cout << "Compression Ratio: " << double(compFileLen_) / fileLen_ << endl;
}