#include "headers.h"

// -----------------------------------------------------------------------------
//  BNode: basic structure of node in b-tree
// -----------------------------------------------------------------------------
BNode::BNode()						// constructor
{
	level_ = -1;
	num_entries_ = -1;
	left_sibling_ = right_sibling_ = -1;
	value_ = nullptr;

	block_ = capacity_ = -1;
	dirty_ = false;
	btree_ = nullptr;
}

// -----------------------------------------------------------------------------
BNode::~BNode()						// destructor
{
	value_ = nullptr;
	btree_ = nullptr;
}

// -----------------------------------------------------------------------------
void BNode::init(					// init a new node, which not exist
	int level,							// level (depth) in b-tree
	BTree* btree)						// b-tree of this node
{
	btree_ = btree;					// init <btree_>
	level_ = static_cast<char>(level);			// init <level_>

	dirty_ = true;					// init <dirty_>
	left_sibling_ = -1;				// init <left_sibling_>
	right_sibling_ = -1;				// init <right_sibling_>
	value_ = nullptr;					// init <value_>

	num_entries_ = 0;				// init <num_entries_>
	block_ = -1;						// init <block_>
	capacity_ = -1;					// init <capacity_>
}

// -----------------------------------------------------------------------------
void BNode::init_restore(			// load an exist node from disk to init
	BTree* btree,						// b-tree of this node
	int block)							// addr of disk for this node
{
	btree_ = btree;					// init <btree_>
	block_ = block;					// init <block_>

	dirty_ = false;					// init <dirty_>
	left_sibling_ = -1;				// init <left_sibling_>
	right_sibling_ = -1;				// init <right_sibling_>
	value_ = nullptr;					// init <value_>

	num_entries_ = 0;				// init <num_entries_>
	level_ = -1;						// init <block_>
	capacity_ = -1;					// init <capacity_>
}

// -----------------------------------------------------------------------------
int BNode::get_entry_size()			// get entry size of b-node
{
	return 0;						// return nothing
}

// -----------------------------------------------------------------------------
void BNode::read_from_buffer(		// do nothing
	char* buf)
{
}

// -----------------------------------------------------------------------------
void BNode::write_to_buffer(		// do nothing
	char* buf)
{
}

// -----------------------------------------------------------------------------
void BNode::find_position_by_key(	// find pos just less than input key
	float key,
	int& low_pos,
	int& high_pos)
{						// do nothing
}

// -----------------------------------------------------------------------------
float BNode::get_key(				// get <key> indexed by <index>
	int index)							// input <index>
{
	return -1.0f;					// do nothing
}

// -----------------------------------------------------------------------------
BNode* BNode::get_left_sibling()	// get the left-sibling node
{
	BNode* node = nullptr;
	if (left_sibling_ != -1) {		// left sibling node exist
		node = new BNode();			// read left-sibling from disk
		node->init_restore(btree_, left_sibling_);
	}
	return node;
}

// -----------------------------------------------------------------------------
BNode* BNode::get_right_sibling()	// get the right-sibling node
{
	BNode* node = nullptr;
	if (right_sibling_ != -1) {		// right sibling node exist
		node = new BNode();			// read right-sibling from disk
		node->init_restore(btree_, right_sibling_);
	}
	return node;
}

// -----------------------------------------------------------------------------
int BNode::get_block() const  // get <block_> (address of this node)
{
	return block_;
}

// -----------------------------------------------------------------------------
int BNode::get_num_entries() const  // get <num_entries_>
{
	return num_entries_;
}

// -----------------------------------------------------------------------------
int BNode::get_level() const // get <level_>
{
	return level_;
}

// -----------------------------------------------------------------------------
//	<level>: SIZECHAR
//	<num_entries> <left_sibling> and <right_sibling>: SIZEINT
// -----------------------------------------------------------------------------
int BNode::get_header_size()		// get header size of b-node
{
	return SIZECHAR + SIZEINT * 3;
}

// -----------------------------------------------------------------------------
float BNode::get_key_of_node() const  // get key of this node
{
	return value_[0];
}

// -----------------------------------------------------------------------------
bool BNode::isFull() const  // whether is full?
{
	return num_entries_ >= capacity_;
}

// -----------------------------------------------------------------------------
void BNode::set_left_sibling(		// set addr of left sibling node
	int left_sibling)					// addr of left sibling node
{
	left_sibling_ = left_sibling;
}

// -----------------------------------------------------------------------------
void BNode::set_right_sibling(		// set addr of right sibling node
	int right_sibling)					// addr of right sibling node
{
	right_sibling_ = right_sibling;
}



// -----------------------------------------------------------------------------
//  BIndexNode: structure of index node for b-tree
// -----------------------------------------------------------------------------
BIndexNode::BIndexNode()			// constructor
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

// -----------------------------------------------------------------------------
BIndexNode::~BIndexNode()			// destructor
{
	char* buf = nullptr;
	if (dirty_) {					// if dirty, rewrite to disk
		int block_length = btree_->file_->get_blocklength();
		buf = new char[block_length];
		write_to_buffer(buf);
		btree_->file_->write_block(buf, block_);

		delete[] buf;
		buf = nullptr;
	}

	if (value_) {						// release <value_>
		delete[] value_; 
		value_ = nullptr;
	}
	if (son_) {						// release <son_>
		delete[] son_; 
		son_ = nullptr;
	}
}

// -----------------------------------------------------------------------------
void BIndexNode::init(				// init a new node, which not exist
	int level,					    // level (depth) in b-tree
	BTree* btree)					// b-tree of this node
{
	btree_ = btree;					// init <btree_>
	level_ = static_cast<char>(level);			// init <level_>

	num_entries_ = 0;				// init <num_entries_>
	left_sibling_ = -1;				// init <left_sibling_>
	right_sibling_ = -1;			// init <right_sibling_>
	dirty_ = true;					// init <dirty_>

									// init <capacity_>
	int b_length = btree_->file_->get_blocklength();
	capacity_ = (b_length - get_header_size()) / get_entry_size();

	if (capacity_ < 50) {			// ensure at least 50 entries
		printf("capacity = %d\n", capacity_);
		error("BIndexNode::init() capacity too small.\n", true);
	}

	value_ = new float[capacity_];	// init <value_>
	for (int i = 0; i < capacity_; i++) {
		value_[i] = MINREAL;
	}
	son_ = new int[capacity_];		// init <son_>
	for (int i = 0; i < capacity_; i++) {
		son_[i] = -1;
	}

	char* blk = new char[b_length];	// init <block_>, get new addr
	block_ = btree_->file_->append_block(blk);
	delete[] blk; 
	blk = NULL;
}

// -----------------------------------------------------------------------------
void BIndexNode::init_restore(		// load an exist node from disk to init
	BTree* btree,						// b-tree of this node
	int block)							// addr of disk for this node
{
	btree_ = btree;					// init <btree_>
	block_ = block;					// init <block_>
	dirty_ = false;					// init <dirty_>

									// get block length
	int b_len = btree_->file_->get_blocklength();

									// init <capacity_>
	capacity_ = (b_len - get_header_size()) / get_entry_size();
	if (capacity_ < 50) {			// at least 50 entries
		printf("capacity = %d\n", capacity_);
		error("BIndexNode::init_restore capacity too small.\n", true);
	}

	value_ = new float[capacity_];	// init <value_>
	for (int i = 0; i < capacity_; i++) {
		value_[i] = MINREAL;
	}
	son_ = new int[capacity_];		// init <son_>
	for (int i = 0; i < capacity_; i++) {
		son_[i] = -1;
	}

	// -------------------------------------------------------------------------
	//  Read the buffer <blk> to init <level_>, <num_entries_>, <left_sibling_>,
	//  <right_sibling_>, <value_> and <son_>.
	// -------------------------------------------------------------------------
	char* blk = new char[b_len];
	btree_->file_->read_block(blk, block);
	read_from_buffer(blk);

	delete[] blk; blk = nullptr;
}

// -----------------------------------------------------------------------------
//  entry: <value_>: SIZEFLOAT and <son_>: SIZEINT
// -----------------------------------------------------------------------------
int BIndexNode::get_entry_size()	// get entry size of b-node
{
	int entry_size = SIZEFLOAT + SIZEINT;
	return entry_size;
}

// -----------------------------------------------------------------------------
//  Read info from buffer to initialize <level_>, <num_entries_>,
//  <left_sibling_>, <right_sibling_>, <value_> and <son_> of b-index node
// -----------------------------------------------------------------------------
void BIndexNode::read_from_buffer(	// read a b-node from buffer
	char* buf)							// store info of a b-index node
{
	int i = 0;						// read <level_>
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
void BIndexNode::write_to_buffer(	// write info of node into buffer
	char* buf)						// store info of this node (return)
{
	int i = 0;						// write <level_>
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

// -----------------------------------------------------------------------------
//  Find position of entry that is just less than or equal to input entry.
//  If input entry is smaller than all entry in this node, we'll return -1.
//  The scan order is from right to left.
// -----------------------------------------------------------------------------
void BIndexNode::find_position_by_key(	// find pos just less than input key
	float key,
	int& low_pos,
	int& high_pos)						
{
									// linear scan (right to left)
	for (int i = num_entries_ - 1; i >= 0; i--) {
		if (value_[i] <= key) {
			high_pos = i;
			break;
		}
	}
	for (low_pos = high_pos; low_pos < num_entries_; ++low_pos)
	{
		if (value_[low_pos] >= key)
		{
			break;
		}
	}
}

// -----------------------------------------------------------------------------
float BIndexNode::get_key(			// get <key> indexed by <index>
	int index)						// input index
{
	if (index < 0 || index >= num_entries_) {
		error("BIndexNode::get_key out of range.", true);
	}
	return value_[index];
}

// -----------------------------------------------------------------------------
									// get the left-sibling node
BIndexNode* BIndexNode::get_left_sibling()
{
	BIndexNode* node = nullptr;
	if (left_sibling_ != -1) {		// left sibling node exist
		node = new BIndexNode();	// read left-sibling from disk
		node->init_restore(btree_, left_sibling_);
	}
	return node;
}

// -----------------------------------------------------------------------------
									// get the right-sibling node
BIndexNode* BIndexNode::get_right_sibling()
{
	BIndexNode* node = nullptr;
	if (right_sibling_ != -1) {		// right sibling node exist
		node = new BIndexNode();	// read right-sibling from disk
		node->init_restore(btree_, right_sibling_);
	}
	return node;
}

// -----------------------------------------------------------------------------
int BIndexNode::get_son(			// get son indexed by <index>
	int index) const  // input index
{
	if (index < 0 || index >= num_entries_) {
		error("BIndexNode::get_son out of range.", true);
	}
	return son_[index];
}

// -----------------------------------------------------------------------------
void BIndexNode::add_new_child(		// add a new entry from its child node
	float key,							// input key
	int son)							// input son
{
	if (num_entries_ >= capacity_) {
		error("BIndexNode::add_new_child overflow", true);
	}

	value_[num_entries_] = key;		// add new entry into its pos
	son_[num_entries_] = son;

	num_entries_++;					// update <num_entries_>
	dirty_ = true;					// node modified, <dirty_> is true
}



// -----------------------------------------------------------------------------
//  BLeafNode: structure of leaf node in b-tree
// -----------------------------------------------------------------------------
BLeafNode::BLeafNode()				// constructor
{
	level_ = -1;
	num_entries_ = -1;
	left_sibling_ = right_sibling_ = -1;

	block_ = capacity_ = -1;
	dirty_ = false;
	btree_ = nullptr;

	num_keys_ = -1;
	capacity_keys_ = -1;
	value_ = nullptr;
	index_ = nullptr;
}

// -----------------------------------------------------------------------------
BLeafNode::~BLeafNode()				// destructor
{
	char* buf = nullptr;
	if (dirty_) {					// if dirty, rewrite to disk
		int block_length = btree_->file_->get_blocklength();
		buf = new char[block_length];
		write_to_buffer(buf);
		btree_->file_->write_block(buf, block_);

		delete[] buf;
		buf = nullptr;
	}

	if (value_) {						// release <value_>
		delete[] value_; 
		value_ = nullptr;
	}
	if (index_) {						// release <index_>
		delete[] index_; 
		index_ = nullptr;
	}
}

// -----------------------------------------------------------------------------
void BLeafNode::init(				// init a new node, which not exist
	int level,							// level (depth) in b-tree
	BTree* btree)						// b-tree of this node
{
	btree_ = btree;					// init <btree_>
	level_ = static_cast<char>(level);	// init <level_>

	num_entries_ = 0;				// init <num_entries_>
	num_keys_ = 0;					// init <num_keys_>
	left_sibling_ = -1;				// init <left_sibling_>
	right_sibling_ = -1;				// init <right_sibling_>
	dirty_ = true;					// init <dirty_>

									// get block length
	int b_length = btree_->file_->get_blocklength();

	// -------------------------------------------------------------------------
	//  Init <capacity_keys_> and calc key size
	// -------------------------------------------------------------------------
	int key_size = get_key_size(b_length);
									// init <key>
	value_ = new float[capacity_keys_];
	for (int i = 0; i < capacity_keys_; i++) {
		value_[i] = MINREAL;
	}
									// calc header size
	int header_size = get_header_size();
									// calc entry size
	int entry_size = get_entry_size();	
									// init <capacity>
	capacity_ = (b_length - header_size - key_size) / entry_size;
	if (capacity_ < 100) {			// at least 100 entries
		printf("capacity = %d\n", capacity_);
		error("BLeafNode::init capacity too small.\n", true);
	}
	index_ = new int[capacity_];		// init <id>
	for (int i = 0; i < capacity_; i++) {
		index_[i] = -1;
	}

	char* blk = new char[b_length];	// init <block>
	block_ = btree_->file_->append_block(blk);
	delete[] blk; blk = nullptr;
}

// -----------------------------------------------------------------------------
void BLeafNode::init_restore(		// load an exist node from disk to init
	BTree* btree,						// b-tree of this node
	int block)							// addr of disk for this node
{
	btree_ = btree;					// init <btree_>
	block_ = block;					// init <block_>
	dirty_ = false;					// init <dirty_>

									// get block length
	int b_length = btree_->file_->get_blocklength();

	// -------------------------------------------------------------------------
	//  Init <capacity_keys> and calc key size
	// -------------------------------------------------------------------------
	int key_size = get_key_size(b_length);
									// init <key>
	value_ = new float[capacity_keys_];
	for (int i = 0; i < capacity_keys_; i++) {
		value_[i] = MINREAL;
	}
									// calc header size
	int header_size = get_header_size();
									// calc entry size
	int entry_size = get_entry_size();	
									// init <capacity>
	capacity_ = (b_length - header_size - key_size) / entry_size;
	if (capacity_ < 100) {			// at least 100 entries
		printf("capacity = %d\n", capacity_);
		error("BLeafNode::init_store capacity too small.\n", true);
	}
	index_ = new int[capacity_];		// init <id>
	for (int i = 0; i < capacity_; i++) {
		index_[i] = -1;
	}

	// -------------------------------------------------------------------------
	//  Read the buffer <blk> to init <level_>, <num_entries_>, <left_sibling_>,
	//  <right_sibling_>, <num_keys_> <value_> and <index_>
	// -------------------------------------------------------------------------
	char* blk = new char[b_length];
	btree_->file_->read_block(blk, block);
	read_from_buffer(blk);

	delete[] blk; blk = nullptr;
}

// -----------------------------------------------------------------------------
int BLeafNode::get_entry_size()		// get entry size in b-node
{
	return SIZEINT;						// <id>: sizeof(int)
}

// -----------------------------------------------------------------------------
void BLeafNode::read_from_buffer(	// read a b-node from buffer
	char* buf)							// store info of a b-node
{
	int i = 0;
	// -------------------------------------------------------------------------
	//  Read header: <level_> <num_entries_> <left_sibling_> <right_sibling_>
	// -------------------------------------------------------------------------
	memcpy(&level_, &buf[i], SIZECHAR);
	i += SIZECHAR;

	memcpy(&num_entries_, &buf[i], SIZEINT);
	i += SIZEINT;

	memcpy(&left_sibling_, &buf[i], SIZEINT);
	i += SIZEINT;

	memcpy(&right_sibling_, &buf[i], SIZEINT);
	i += SIZEINT;

	// -------------------------------------------------------------------------
	//  Read keys: <num_keys_> and <value_>
	// -------------------------------------------------------------------------
	memcpy(&num_keys_, &buf[i], SIZEINT);
	i += SIZEINT;

	for (int j = 0; j < capacity_keys_; j++) {
		memcpy(&value_[j], &buf[i], SIZEFLOAT);
		i += SIZEFLOAT;
	}

	// -------------------------------------------------------------------------
	//  Read entries: <index_>
	// -------------------------------------------------------------------------
	for (int j = 0; j < num_entries_; j++) {
		memcpy(&index_[j], &buf[i], SIZEINT);
		i += SIZEINT;
	}
}

// -----------------------------------------------------------------------------
void BLeafNode::write_to_buffer(	// write a b-node into buffer
	char* buf)							// store info of a b-node
{
	int i = 0;
	// -------------------------------------------------------------------------
	//  Write header: <level_> <num_entries_> <left_sibling_> <right_sibling_>
	// -------------------------------------------------------------------------
	memcpy(&buf[i], &level_, SIZECHAR);
	i += SIZECHAR;

	memcpy(&buf[i], &num_entries_, SIZEINT);
	i += SIZEINT;

	memcpy(&buf[i], &left_sibling_, SIZEINT);
	i += SIZEINT;

	memcpy(&buf[i], &right_sibling_, SIZEINT);
	i += SIZEINT;

	// -------------------------------------------------------------------------
	//  Write keys: <num_keys_> and <value_>
	// -------------------------------------------------------------------------
	memcpy(&buf[i], &num_keys_, SIZEINT);
	i += SIZEINT;

	for (int j = 0; j < capacity_keys_; j++) {
		memcpy(&buf[i], &value_[j], SIZEFLOAT);
		i += SIZEFLOAT;
	}

	// -------------------------------------------------------------------------
	//  Write entries: <index_>
	// -------------------------------------------------------------------------
	for (int j = 0; j < num_entries_; j++) {
		memcpy(&buf[i], &index_[j], SIZEINT);
		i += SIZEINT;
	}
}

// -----------------------------------------------------------------------------
void BLeafNode::find_position_by_key(	// find pos just less than input key
	float key,
	int& low_pos,
	int& high_pos)						
{
	// linear scan (right to left)
	for (int i = num_entries_ - 1; i >= 0; i--) {
		if (value_[i] <= key) {
			high_pos = i;
			break;
		}
	}
	for (low_pos = high_pos; low_pos < num_entries_; ++low_pos)
	{
		if (value_[low_pos] >= key)
		{
			break;
		}
	}
}

// -----------------------------------------------------------------------------
float BLeafNode::get_key(			// get <value_> indexed by <index>
	int index)							// input <index>
{
	if (index < 0 || index >= num_keys_) {
		error("BLeafNode::get_key out of range.", true);
	}
	return value_[index];
}

// -----------------------------------------------------------------------------
									// get left-sibling node
BLeafNode* BLeafNode::get_left_sibling()
{
	BLeafNode* node = nullptr;
	if (left_sibling_ != -1) {		// left sibling node exist
		node = new BLeafNode();		// read left-sibling from disk
		node->init_restore(btree_, left_sibling_);
	}
	return node;
}

// -----------------------------------------------------------------------------
									// get right sibling node
BLeafNode* BLeafNode::get_right_sibling()
{
	BLeafNode* node = nullptr;
	if (right_sibling_ != -1) {		// right sibling node exist
		node = new BLeafNode();		// read right-sibling from disk
		node->init_restore(btree_, right_sibling_);
	}
	return node;
}

// -----------------------------------------------------------------------------
int BLeafNode::get_key_size(		// get key size of this node
	int _block_length)				// block length
{
	capacity_keys_ = static_cast<int>(ceil(static_cast<float>(_block_length) / INDEX_SIZE_LEAF_NODE));

	// -------------------------------------------------------------------------
	//  Array of <value_> with number <capacity_keys_> + <number_keys_> (SIZEINT)
	// -------------------------------------------------------------------------
	int key_size = capacity_keys_ * SIZEFLOAT + SIZEINT;
	return key_size;
}

// -----------------------------------------------------------------------------
int BLeafNode::get_increment()		// get <increment>
{
	int entry_size = get_entry_size();
	int increment = INDEX_SIZE_LEAF_NODE / entry_size;

	return increment;
}

// -----------------------------------------------------------------------------
int BLeafNode::get_num_keys() const  // get <num_keys_>
{
	return num_keys_;
}

// -----------------------------------------------------------------------------
int BLeafNode::get_entry_id(		// get entry id indexed by <index>
	int index) const  // input <index>
{
	if (index < 0 || index >= num_entries_) {
		error("BLeafNode::get_entry_id out of range.", true);
	}
	return index_[index];
}

// -----------------------------------------------------------------------------
void BLeafNode::add_new_child(		// add new child by input index and value
	int index,								// input object index
	float value)							// input value
{
	if (num_entries_ >= capacity_) {
		error("BLeafNode::add_new_child entry overflow", true);
	}

	index_[num_entries_] = index;			// add new index into its pos

	//if ((num_entries_ * SIZEINT) % INDEX_SIZE_LEAF_NODE == 0) {
	//	if (num_keys_ >= capacity_keys_) {
	//		error("BLeafNode::add_new_child value overflow", true);
	//	}

	value_[num_keys_] = value; 		// add new value into its pos
	num_keys_++;				    // update <num_keys>
	//}

	num_entries_++;					// update <num_entries>
	dirty_ = true;					// node modified, <dirty> is true
}


