#include "decoder.hpp"

using namespace std;

Decoder::Decoder(string filename): 
    filename_{filename}, huffmanTree_{new HuffmanNode{nullptr, nullptr, 0}}, fileLen_{0}, numChars_{0} {
    fstream codesIn{filename + ".compress.codes"};
    if (codesIn.is_open()){
        string buf;
        getline(codesIn, buf);
        numChars_ = stoull(buf);
        getline(codesIn, buf);
        fileLen_ = stoull(buf);
        while (codes_.size() < numChars_){
            short c;
            string prefix;
            codesIn >> c;
            codesIn >> prefix;
            codes_[prefix] = c;
        }
    } else {
        cerr << "Couldn't open file" << endl;
        exit(1);
    }
    
}

Decoder::~Decoder()
{
    destructorHelper(huffmanTree_);
}


void Decoder::destructorHelper(HuffmanNode *&node){
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
// Build Tree
void Decoder::buildTree(){
    for (auto [prefixCode, ch] : codes_){
        insertChar(huffmanTree_, prefixCode, 0, ch);
    }
}

void Decoder::insertChar(HuffmanNode*& curNode, const string& prefixCode, size_t codeInd, char c){
    // Base case. 
    if (codeInd == prefixCode.length() - 1){
        if (prefixCode.back() == '1'){
            curNode->right_ = new HuffmanNode(c, 0); // frequencies are irrelevant here. 
        } else {
            curNode->left_ = new HuffmanNode(c, 0);
        }
    } else {
        // Recursive Cases: 
        if (prefixCode[codeInd] == '1'){
            if (!curNode->right_){
                curNode->right_ = new HuffmanNode(nullptr, nullptr, 0);
            }
            insertChar(curNode->right_, prefixCode, codeInd + 1, c);
        } else
        {
            if (!curNode->left_){
                curNode->left_ = new HuffmanNode(nullptr, nullptr, 0);
            }
            insertChar(curNode->left_, prefixCode, codeInd + 1, c);
        }
    }
} //0100010, 011000

string Decoder::getString(){ // returns a BIG STRING
    fstream data{filename_ + ".compress"};
    unsigned char c;
    string fullCode;
    if (data.is_open()) {
        while (data >> noskipws >> c){
            string next;
            for (size_t i = 0; i < 8; ++i)
            {
                if (c%2 == 0){
                    next += '0';
                } else {
                    next += '1';
                }
                c >>= 1;
            }
            reverse(begin(next), end(next));
            fullCode += next;
        }
    }
    return fullCode;
}

char Decoder::decodeString(std::string::iterator& iter){
    HuffmanNode *curNode = huffmanTree_;
    while (!curNode->isLeaf()){
        // shouldn't go past the end!
        if (*iter == '0'){
            curNode = curNode->left_;
        } else {
            curNode = curNode->right_;
        }
        ++iter;
    }
    return curNode->symbol_;
}

void Decoder::writeToFile(string& binaryStr){
    // Take the massive string
    std::string::iterator iter = begin(binaryStr);
    ofstream out{filename_ + ".uncompress"};
    for (size_t i = 0; i < fileLen_; ++i){
        char c = decodeString(iter);
        out << c;
    }
    out << flush;
    return;
}

void Decoder::Decode(){
    buildTree();
    string s = getString();

    writeToFile(s);
    return;
}