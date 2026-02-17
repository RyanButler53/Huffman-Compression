#ifndef COMPRESS_HPP_INCLUDED
#define COMPRESS_HPP_INCLUDED

#include "huffmanNode.hpp"
#include <string>
#include <unordered_map>

#include "threadsafeQueue.hpp"

#include "threadsafeQueue.hpp"

#ifdef HC_WITH_GPU
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#endif


namespace execution{
  enum class space : uint8_t {cpu = 0, async = 1, gpu = 2};
}
class Encoder
{
  private:
    HuffmanNode* huffmanTree_;
    size_t fileLen_;
    size_t compFileLen_;


    void destructorHelper(HuffmanNode *&node);
    void codesHelper(const std::string &currentCode, HuffmanNode* curNode, std::array<std::string, 256>& codes);
    void buildFromFreq(std::array<unsigned long, 256> freqs);
    std::array<std::string, 256> getCodes();

    // 3 helper methods for writeToFile() method 
    virtual void getCompressedString(std::string& compressedString, std::array<std::string, 256>& codes);
    virtual void getCompressedBytes(std::vector<unsigned char>& compressedChars, std::string& compressedString);
    virtual void writeToFile(std::vector<unsigned char>& compressedChars);
  
    // Main writeToFile() compress function
    virtual void writeToFile(std::array<std::string, 256>& codes);
    void writeCodes(std::array<std::string, 256>& codes);

  protected:
    std::string filename_;

  public:
    Encoder(std::string file);
    virtual ~Encoder();

    void Encode();
    std::tuple<size_t, size_t, double> getStats() const;
    static std::unique_ptr<Encoder> make(execution::space space, std::string filename);
};

#endif
