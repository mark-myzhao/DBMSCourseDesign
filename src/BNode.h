#ifndef _BNODE_H_
#define _BNODE_H_
#include "def.h"

class BNode {
public:
    BNode();
    ~BNode();

    void writeToFile(char* fileName, POSPOINTER addr) const;
    void restoreFrom(char* fileName, POSPOINTER addr);

    int getEntrySize() const;
                              //  There is a doubly linked lists between nodes each level.
    virtual BNode* getLastSibling() const;  // get last sibling node
    virtual BNode* getNextSibling() const;  // get next sibling node
    /* data */
protected:
    int level_;        //  depth of this node. if level = 0 || 1 then this node is an index node; 
                       //                      else it is a leaf node.
    BTree* btree_;     //  point to the tree which this node belong to; also point out which file it belongs to
    bool dirty_;       //  mark if this node has been modified
    int entriesNum_;    //  number of entries
    float* key_;       //  keys in this node
    POSPOINTER addr_;   //  position pointer of this node
    
};

class BIndexNode : public BNode {
public:
    BIndexNode();
    ~BIndexNode();

    virtual BIndexNode* getLastSibling() const;  // get last sibling node
    virtual BIndexNode* getNextSibling() const;  // get next sibling node
    /* data */
private:
    POSPOINTER child_;            //  position pointer
};

class BLeafNode : public BNode {
public:
    BLeafNode();
    ~BLeafNode();

    virtual BLeafNode* getLastSibling() const;  // get last sibling node
    virtual BLeafNode* getNextSibling() const;  // get next sibling node
    int getInde() const;
    /* data */
private:
    int indexFromLItem_;  //  index
};


#endif