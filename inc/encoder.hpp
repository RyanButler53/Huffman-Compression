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
    void codesHelper(const std::string &currentCode, HuffmanNode* curNode, std::unordered_map<char, std::string>& codes);
    void buildFromFreq(std::unordered_map<char, unsigned long> freqs);
    std::unordered_map<char, std::string> getCodes();

    void getCompressedString(std::string& compressedString, std::unordered_map<char, std::string>& codes);
    void getCompressedBytes(std::vector<unsigned char>& compressedChars, std::string& compressedString);
    void writeToFile(std::vector<unsigned char>& compressedChars);
  
    void writeToFile(std::unordered_map<char, std::string>& codes);
    void writeCodes(std::unordered_map<char, std::string> &codes);

  public:
    Encoder(std::string file);
    ~Encoder();

    void Encode();
};

#endif
