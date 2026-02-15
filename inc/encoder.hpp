#ifndef COMPRESS_HPP_INCLUDED
#define COMPRESS_HPP_INCLUDED

#include "huffmanNode.hpp"
#include <string>
#include <unordered_map>

class Encoder
{
  private:
    std::string filename_;
    HuffmanNode* huffmanTree_;
    size_t fileLen_;
    size_t compFileLen_;

    void destructorHelper(HuffmanNode *&node);
    void codesHelper(const std::string &currentCode, HuffmanNode* curNode, std::array<std::string, 256>& codes);
    void buildFromFreq(std::array<unsigned long, 256> freqs);
    std::array<std::string, 256> getCodes();

    void getCompressedString(std::string& compressedString, std::array<std::string, 256>& codes);
    void getCompressedBytes(std::vector<unsigned char>& compressedChars, std::string& compressedString);
    void writeToFile(std::vector<unsigned char>& compressedChars);
  
    void writeToFile(std::array<std::string, 256>& codes);
    void writeCodes(std::array<std::string, 256>& codes);

  public:
    Encoder(std::string file);
    ~Encoder();

    void Encode();
};

#endif
