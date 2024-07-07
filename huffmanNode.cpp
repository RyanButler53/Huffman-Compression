#include "huffmanNode.hpp"
#include <iostream>

// Leaf nodes
HuffmanNode::HuffmanNode(unsigned char symbol, unsigned long freq):
left_{nullptr},right_{nullptr}, symbol_{symbol}, freq_{freq} 
{
    //nothing to do here
}

// Internal Nodes
HuffmanNode::HuffmanNode(HuffmanNode* left, HuffmanNode* right, unsigned long freq):
left_{left}, right_{right}, symbol_{0}, freq_{freq}
{

}

bool HuffmanNode::isLeaf() const {
    return left_ == nullptr and right_ == nullptr;
}

std::ostream& operator<<(std::ostream& os, const HuffmanNode& h) {
    os << "[" << h.symbol_ << ", " << h.freq_ << "]";
    return os;
}

bool HuffmanPtrCompare::operator()(HuffmanNode* h1, HuffmanNode* h2){
    return h1->freq_ > h2->freq_;
}
