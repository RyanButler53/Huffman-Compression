#ifndef HUFFMANNODE_HPP_INCLUDED
#define HUFFMANNODE_HPP_INCLUDED

#include <iostream>

struct HuffmanNode
{
    // Data
    HuffmanNode *left_;
    HuffmanNode *right_;
    unsigned char symbol_;
    unsigned long freq_;

    HuffmanNode() = default;
    HuffmanNode(unsigned char symbol, unsigned long freq); // Leaf
    HuffmanNode(HuffmanNode *left, HuffmanNode *right, unsigned long freq); // Internal

    HuffmanNode(const HuffmanNode &other) = default;
    ~HuffmanNode() = default;

    bool isLeaf() const;
    // bool operator>(const HuffmanNode &other) const;
    // bool operator< (const HuffmanNode& other) const;

};

struct HuffmanPtrCompare{
    bool operator()( HuffmanNode *h1, HuffmanNode *h2);
};

std::ostream &operator<<(std::ostream &os, const HuffmanNode &h);

#endif
