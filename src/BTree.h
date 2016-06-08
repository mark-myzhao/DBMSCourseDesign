#ifndef _BTREE_H_
#define _BTREE_H_
#include "def.h"

class BTree {
public:
    BTree();
    ~BTree();

    BlockFile* getBlockFile() const;
    BNode* getRootPtr() const;

    void setBlockFile(BlockFile* file);
    void setRootPtr(BNode* root);

    void storeTree(char* fileName, int blockSize) const;
    void loadTree(char* fileName);

    void searchInTree(int target, int & min, int & max) const;  //  find the upper and lower bound of target

    void bulkload(char* fileName);  //  bulkload a B+ tree from the file
private:
    BlockFile* file_;     //  the file which b+ tree is stored in
    BNode* rootPtr_;     //  pointer to the root

};

#endif