#include "b_node.h"
#include "b_tree.h"
#include "block_file.h"
#include "cmath"
#include <cstring>

BNode::BNode()                      // constructor
{
    level_ = -1;
    num_entries_ = -1;
    left_sibling_ = right_sibling_ = -1;

    block_ = capacity_ = -1;
    dirty_ = false;
    btree_ = nullptr;

    value_ = nullptr;
    son_ = nullptr;
}

BNode::~BNode()                     // destructor
{
    char* buf = nullptr;
    if (dirty_) {                   // if dirty, rewrite to disk
        int block_length = btree_->file_->get_blocklength();
        buf = new char[block_length];
        write_to_buffer(buf);
        btree_->file_->write_block(buf, block_);

        delete[] buf;
        buf = nullptr;
    }

    if (value_) {                   // release <value_>
        delete[] value_; 
        value_ = nullptr;
    }
    if (son_) {                     // release <son_>
        delete[] son_; 
        son_ = nullptr;
    }
}

void BNode::init(                   // init a new node, which contains no entry
    int level,                          // level (depth) in b-tree
    BTree* btree)                       // b-tree of this node
{
    btree_ = btree;                 // init <btree_>
    level_ = static_cast<char>(level);  // init <level_>

    num_entries_ = 0;               // init <num_entries_>
    left_sibling_ = -1;             // init <left_sibling_>
    right_sibling_ = -1;            // init <right_sibling_>
    dirty_ = true;                  // init <dirty_>

                                    // init <capacity_>
    int b_length = btree_->file_->get_blocklength();
    capacity_ = (b_length - get_header_size()) / get_entry_size();

    if (capacity_ < 50) {           // ensure at least 50 entries
        printf("capacity = %d\n", capacity_);
        error("BNode::init() capacity too small.\n", true);
    }

    value_ = new float[capacity_];  // init <value_>
    for (int i = 0; i < capacity_; i++) {
        value_[i] = MINREAL;
    }
    son_ = new int[capacity_];      // init <son_>
    for (int i = 0; i < capacity_; i++) {
        son_[i] = -1;
    }

    char* blk = new char[b_length];
    // init <block_>, get new addr
    block_ = btree_->file_->append_block(blk);
    delete[] blk; 
    blk = nullptr;
}

void BNode::init_restore(           // load an exist node from disk to init
    BTree* btree,                   // b-tree of this node
    int block)                      // addr of disk for this node
{
    btree_ = btree;                 // init <btree_>
    block_ = block;                 // init <block_>
    dirty_ = false;                 // init <dirty_>

                                    // get block length
    int b_len = btree_->file_->get_blocklength();

                                    // init <capacity_>
    capacity_ = (b_len - get_header_size()) / get_entry_size();
    if (capacity_ < 50) {           // at least 50 entries
        printf("capacity = %d\n", capacity_);
        error("BNode::init_restore capacity too small.\n", true);
    }

    value_ = new float[capacity_];  // init <value_>
    for (int i = 0; i < capacity_; i++) {
        value_[i] = MINREAL;
    }
    son_ = new int[capacity_];      // init <son_>
    for (int i = 0; i < capacity_; i++) {
        son_[i] = -1;
    }

    //  Read the buffer <blk> to init <level_>, <num_entries_>, <left_sibling_>,
    //  <right_sibling_>, <value_> and <son_>.
    char* blk = new char[b_len];
    btree_->file_->read_block(blk, block);
    read_from_buffer(blk);

    delete[] blk; 
    blk = nullptr;
}

int BNode::get_entry_size() const   // get entry size of b-node
{
    int entry_size = SIZEFLOAT + SIZEINT;
    return entry_size;
}

// -----------------------------------------------------------------------------
void BNode::read_from_buffer(       // parse the block
    char* buf)
{
    int i = 0;                      // read <level_>
    memcpy(&level_, &buf[i], SIZECHAR);
    i += SIZECHAR;
                                    // read <num_entries_>
    memcpy(&num_entries_, &buf[i], SIZEINT);
    i += SIZEINT;
                                    // read <left_sibling_>
    memcpy(&left_sibling_, &buf[i], SIZEINT);
    i += SIZEINT;
                                    // read <right_sibling_>
    memcpy(&right_sibling_, &buf[i], SIZEINT);
    i += SIZEINT;

    for (int j = 0; j < num_entries_; j++) {
                                    // read <value_>
        memcpy(&value_[j], &buf[i], SIZEFLOAT);
        i += SIZEFLOAT;
                                    // read <son_>
        memcpy(&son_[j], &buf[i], SIZEINT);
        i += SIZEINT;
    }
}

// -----------------------------------------------------------------------------
void BNode::write_to_buffer(
    char* buf)
{
    int i = 0;                      // write <level_>
    memcpy(&buf[i], &level_, SIZECHAR);
    i += SIZECHAR;
                                    // write <num_entries_>
    memcpy(&buf[i], &num_entries_, SIZEINT);
    i += SIZEINT;
                                    // write <left_sibling_>
    memcpy(&buf[i], &left_sibling_, SIZEINT);
    i += SIZEINT;
                                    // write <right_sibling_>
    memcpy(&buf[i], &right_sibling_, SIZEINT);
    i += SIZEINT;

    for (int j = 0; j < num_entries_; j++) {
                                    // write <value_>
        memcpy(&buf[i], &value_[j], SIZEFLOAT);
        i += SIZEFLOAT;
                                    // write <son_>
        memcpy(&buf[i], &son_[j], SIZEINT);
        i += SIZEINT;
    }
}

float BNode::get_key(               // get <key> indexed by <index>
    int index) const                    // input <index>
{
    if (index < 0 || index >= num_entries_) {
        error("BNode::get_key out of range.", true);
    }
    return value_[index];
}

BNode* BNode::get_left_sibling() const // get the left-sibling node
{
    BNode* node = nullptr;
    if (left_sibling_ != -1) {      // left sibling node exist
        node = new BNode();    // read left-sibling from disk
        node->init_restore(btree_, left_sibling_);
    }
    return node;
}

BNode* BNode::get_right_sibling() const // get the right-sibling node
{
    BNode* node = nullptr;
    if (right_sibling_ != -1) {     // right sibling node exist
        node = new BNode();    // read right-sibling from disk
        node->init_restore(btree_, right_sibling_);
    }
    return node;
}

int BNode::get_son(         // get son indexed by <index>
    int index) const        // input index
{
    if (index < 0 || index >= num_entries_) {
        error("BNode::get_son out of range.", true);
    }
    return son_[index];
}

void BNode::add_new_child(      // add a new entry from its child node
    float key,                          // input key
    int son)                            // input son
{
    if (num_entries_ >= capacity_) {
        error("BNode::add_new_entry overflow", true);
    }

    value_[num_entries_] = key;     // add new entry into its pos
    son_[num_entries_] = son;

    num_entries_++;                 // update <num_entries_>
    dirty_ = true;                  // node modified, <dirty_> is true
}

int BNode::get_block() const  // get <block_> (address of this node)
{
    return block_;
}

int BNode::get_num_entries() const  // get <num_entries_>
{
    return num_entries_;
}

int BNode::get_level() const // get <level_>
{
    return level_;
}

//  <level>: SIZECHAR
//  <num_entries> <left_sibling> and <right_sibling>: SIZEINT
int BNode::get_header_size()   // get header size of b-node
{
    return SIZECHAR + SIZEINT * 3;
}

float BNode::get_key_of_node() const  // get key of this node
{
    return value_[0];
}

bool BNode::isFull() const {
    return num_entries_>= capacity_;
}

float BNode::get_index(int index) const {
    return value_[index];
}



void BNode::set_left_sibling(       // set addr of left sibling node
    int left_sibling)                   // addr of left sibling node
{
    left_sibling_ = left_sibling;
}

void BNode::set_right_sibling(      // set addr of right sibling node
    int right_sibling)                  // addr of right sibling node
{
    right_sibling_ = right_sibling;
}