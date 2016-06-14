#ifndef __B_NODE_H
#define __B_NODE_H

class BlockFile;
class BTree;

//  if level_ == 0 -> Leaf, son[i] is LItem[0]
//  else son[i] is the block num of its son node
class BNode {
public:
    BNode();
    ~BNode();

    void init(              // init a new node, which not exist
        int level,                      // level (depth) in b-tree
        BTree* btree);                  // b-tree of this node

    void init_restore(      // load an exist node from disk to init
        BTree* btree,                   // b-tree of this node
        int block);                     // address of file of this node

    void read_from_buffer(  // read a b-node from buffer
        char* buf);                     // store info of a b-node

    void write_to_buffer(   // write a b-node into buffer
        char* buf);                     // store info of a b-node

    int get_entry_size() const;         // get entry size in b-node
                                        // find pos just less than input key

    float get_key(                      // get <value_> indexed by <index>
        int index) const;               // index

    BNode* get_left_sibling() const;
                                        // get right sibling node
    BNode* get_right_sibling() const;

    int get_block() const;

    int get_num_entries() const;

    int get_level() const; 

    static int  get_header_size();       // get header size in b-node

    float get_key_of_node() const;       // get key of this node

    bool isFull() const;                 // whether is full?

    // int get_key_size(                    // get key size of this node
    //     int block_length) const;         // block length

    // int get_num_keys() const;               // get <num_keys_>

    float get_index(                // get entry id indexed by <index>
        int index) const;                   // index

    int get_son(                     // get <son_> indexed by <index>
        int index) const;                       // index

    // void add_new_child(              // add new child by input id and key
    //     int id,                         // input object id
    //     float key);                     // input key

    void add_new_child(
        float key,
        int son);

    void set_left_sibling(           // set <left_sibling>
        int left_sibling);              // addr of left sibling node
 
    void set_right_sibling(          // set <right sibling>
        int right_sibling);             // addr of right sibling node

private:
    // disk params
    char   level_;          //  level of b-tree (level > 0)
    int    num_entries_;    //  number of entries in this node
    int    left_sibling_;   //  addr in disk for left  sibling
    int    right_sibling_;  //  addr in disk for right sibling
    float* value_;          //  keys
    int*   son_;            //  addr of son node or object id

    // other params
    bool   dirty_;          // if dirty, write back to file
    int    block_;          // addr of disk for this node
    int    capacity_;       // max num of entries can be stored
    BTree* btree_;          // b-tree of this node
};

#endif
