#ifndef COMPRESS_HPP_INCLUDED
#define COMPRESS_HPP_INCLUDED

#include "huffmanNode.hpp"
#include <string>
#include <unordered_map>

#include "threadsafeQueue.hpp"

class Encoder
{
  private:
    std::string filename_;
    HuffmanNode* huffmanTree_;
    size_t fileLen_;
    size_t compFileLen_;

    // Async operations 

    class Async {

      ThreadsafeQueue<std::pair<std::string, bool>> compressQueue_;
      ThreadsafeQueue<std::pair<std::vector<unsigned char>, bool>> writeQueue_;
      const std::array<std::string, 256>& codes_;
      std::string filename_;


      size_t readThread(); // returns the total file size
      size_t compressThread(); // returns the compressed file size
      void writeThread();

      public:

      Async(const std::array<std::string, 256>& codes, std::string filename);
      std::pair<size_t, size_t> launch(); 
    };


    void destructorHelper(HuffmanNode *&node);
    void codesHelper(const std::string &currentCode, HuffmanNode* curNode, std::array<std::string, 256>& codes);
    void buildFromFreq(std::array<unsigned long, 256> freqs);
    std::array<std::string, 256> getCodes();

    void getCompressedString(std::string& compressedString, std::array<std::string, 256>& codes);
    void getCompressedBytes(std::vector<unsigned char>& compressedChars, std::string& compressedString);
    void writeToFile(std::vector<unsigned char>& compressedChars);
    void writeToFileAsync(const std::array<std::string, 256>& codes);
  
    void writeToFile(std::array<std::string, 256>& codes);
    void writeCodes(std::array<std::string, 256>& codes);

  public:
    Encoder(std::string file);
    ~Encoder();

    void Encode();
};

#endif
