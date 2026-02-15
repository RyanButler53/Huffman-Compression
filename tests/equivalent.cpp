#include <gtest/gtest.h>
#include <string>
#include <filesystem>
#include <array>

#include "encoder.hpp"
#include "decoder.hpp"

#include <openssl/evp.h>


class EncodeDecodeEquivalence : public ::testing::Test {

    protected:
    std::filesystem::path dataDir_;

    void SetUp() override {
        std::string dataSource = TEST_FILE_DIR;
        dataDir_ = dataSource / std::filesystem::path("sample-text");

    }

    void TearDown() override {
        std::array<std::string, 3> files{"small.txt", "huffman.txt", "smaller.txt"};
        for (auto f : files){
            std::filesystem::remove(dataDir_ / (f + ".compress"));
            std::filesystem::remove(dataDir_ / (f + ".compress.codes"));
            std::filesystem::remove(dataDir_ / (f + ".uncompress"));
        }
    }

    void testFile(std::string fname){
        std::filesystem::path file = dataDir_ / fname;
        
        Encoder e(file);
        e.Encode();
        Decoder d(file);
        d.Decode();
    
        std::string originalHash = hashFile(file);
        std::string compressed = hashFile(dataDir_ / (fname + ".uncompress"));
    
        ASSERT_EQ(originalHash, compressed);
    }

    std::string hashFile(std::filesystem::path filename){
        size_t fsize = std::filesystem::file_size(filename);
        std::vector<char> filedata(fsize);
        // Treat it like a binary file
        std::fstream in(filename, std::ios::binary);
        in.read(filedata.data(), fsize);
        return hashBytes(filedata.data(), fsize);
        
    }

    std::string hashBytes(char* data, size_t size){
        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int numHashedBytes;
        EVP_MD_CTX* mdctx = EVP_MD_CTX_new();

        if (!EVP_DigestInit_ex2(mdctx, EVP_sha256(), NULL)){
            EVP_MD_CTX_free(mdctx);
            throw std::logic_error("Error initializing Hash function");
        }

        if(!EVP_DigestUpdate(mdctx, data, size)) {
            EVP_MD_CTX_free(mdctx);
            throw std::logic_error("Error updating Hash");
        }
        if (!EVP_DigestFinal_ex(mdctx, hash, &numHashedBytes)) {
            printf("Message digest finalization failed.\n");
            EVP_MD_CTX_free(mdctx);
            throw std::logic_error("Hashing Error");
        }

        std::stringstream ss;
        for(int i = 0; i < numHashedBytes; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }

        EVP_MD_CTX_free(mdctx);

        return ss.str();
    }

};

TEST_F(EncodeDecodeEquivalence, smaller){
    testFile("smaller.txt");
}

TEST_F(EncodeDecodeEquivalence, small){
    testFile("small.txt");
}

TEST_F(EncodeDecodeEquivalence, huffman){
    testFile("huffman.txt");
}