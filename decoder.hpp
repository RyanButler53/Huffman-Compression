#ifndef DECODER_HPP_INCLUDED
#define DECODER_HPP_INCLUDED

#include <string>
#include <fstream>
#include "huffmanNode.hpp"
#include <unordered_map>

class Decoder {

  private:
    std::unordered_map<std::string, char> codes_;
    std::string filename_;
    std::string compressedString;
    HuffmanNode *huffmanTree_;
    size_t fileLen_;
    size_t numChars_;

    void destructorHelper(HuffmanNode *&node);
    void insertChar(HuffmanNode *&curNode, const std::string& prefixCode, size_t codeInd, char c);
    void buildTree();
    std::string getString();
    void writeToFile(std::string& binaryStr);
    char decodeString(std::string::iterator& iter);

public:
    Decoder(std::string filename);
    ~Decoder();

    void Decode();
    
};

#endif