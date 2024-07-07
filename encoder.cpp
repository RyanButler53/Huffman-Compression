#include "encoder.hpp"
#include <queue>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

Encoder::Encoder(std::string file):
filename_{file}, huffmanTree_{nullptr}, fileLen_{0}, compFileLen_{0}
{
    // Read in from file, build a huffman tree of frequencies. 
    fstream input{filename_};
    std::unordered_map<char, unsigned long> counts;
    char c;
    fileLen_ = 0;
    if (input.is_open()) {
        while (input >> noskipws >> c){
            if (counts.find(c) != counts.end()){
                ++counts[c];
            } else {
                counts[c] = 1;
            }
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

void Encoder::codesHelper(const string& currentCode, HuffmanNode* curNode, unordered_map<char, std::string>& codes){
    if (curNode->isLeaf()){
        char symbol = curNode->symbol_;
        codes[symbol] = currentCode;
    } else {
        codesHelper(currentCode + "0", curNode->left_, codes);
        codesHelper(currentCode + "1", curNode->right_, codes);
    }
}

void Encoder::buildFromFreq(std::unordered_map<char, unsigned long> freqs){
    priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, HuffmanPtrCompare> q;
    size_t n = freqs.size() - 1; // number of internal nodes
    for (auto &[c, freq] : freqs) {
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

std::unordered_map<char, string> Encoder::getCodes(){
    std::unordered_map<char, string> codes;
    codesHelper("", huffmanTree_, codes);
    return codes;
}

void Encoder::writeToFile(unordered_map<char, string>& codes){
    // Have to read in the REAL FILE DATA
    string compressedString;
    fstream input{filename_};
    char c;
    if (input.is_open()) {
        while (input >> noskipws >> c){
            string prefixCode = codes[c];
            compressedString.append(prefixCode);
        }
        while (compressedString.length() %8 != 0){
            compressedString.append("1");
        }
        
        // turn into bytes.
        vector<unsigned char> compressedChars;
        string::iterator it = begin(compressedString);
        while(it != end(compressedString)){
            char ch = 0;
            for (size_t i = 0; i < 8; ++i){
                ch <<= 1;
                if (*it == '1'){
                    ++ch;
                }
                ++it;
            }
            compressedChars.push_back(ch);
        }
        compFileLen_ = compressedChars.size();
        // Print to file
        ofstream out{filename_ + ".compress"};
        for (unsigned char& c : compressedChars){
            out << c;
        }
    }
    else
    {
        cerr << "Unable to read file" << endl;
        exit(1);
    }
}

void Encoder::writeCodes(unordered_map<char, string> &codes){
    ofstream out{filename_ + ".compress.codes"};
    out << codes.size() << endl;
    out << fileLen_ << endl;
    for (auto& [c, code]:codes){
        out << int(c) << " " << code << endl;
    }
}

void Encoder::Encode(){
    unordered_map<char, string> codes = getCodes();
    writeToFile(codes);
    writeCodes(codes);

    cout << "Unique Characters: " << codes.size() << endl;
    cout << "Original File Size: " << fileLen_ << " bytes" << endl;
    cout << "Compressed File Size: " << compFileLen_ << " bytes" << endl;
    cout << "Compression Ratio: " << double(compFileLen_) / fileLen_ << endl;
}