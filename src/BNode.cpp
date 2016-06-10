#include "BNode.h"

//  -------------
//  Class BNode
BNode::BNode()  {    //  constructor
    level_ = -1;
    btree_ = NULL;
    dirty_ = false;
    entriesNum_ = 0;
    key_ = NULL;
    addr_ = -1;
    lastSibling_ = nextSibling_ = -1;
}

BNode::~BNode() {    //  destructor
    key_ = NULL;
    btree_ = NULL;
}

void BNode::writeToFile(POSPOINTER addr) const {
    //  do nothing
}

void BNode::restoreFrom(BTree* btree, POSPOINTER addr) {
    btree_ = btree;
    addr_ = addr;
    dirty_ = false;
    level_ = -1;
    entriesNum_ = 0;
    key_ = NULL;
    lastSibling_ = nextSibling_ = -1;
}

bool BNode::isDirty() const { return dirty_; }

int BNode::getEntryNum() const { return entriesNum_; }

int BNode::getAddr() const { return addr_; }

int BNode::getLevel() const { return level_; }

//  store Lever; entriesNum; lastSibling; nextSibling in header
int BNode::getHeaderSize() const {
    return SIZEINT * 4;
}

float BNode::getKey() const { 
    if (key_ != NULL)
        return key_[0]; 
    else 
        return -1;
}

BNode* BNode::getLastSibling() const {
    BNode* node = NULL;
    if (left_sibling_ != -1) {      // left sibling node exist
        node = new BNode();         // read left-sibling from disk
        node->init_restore(btree_, left_sibling_);
    }
    return node;
}

BNode* BNode::getNextSibling() const {
    BNode* node = NULL;
    if (right_sibling_ != -1) {     // right sibling node exist
        node = new BNode();         // read right-sibling from disk
        node->init_restore(btree_, right_sibling_);
    }
    return node;
}


