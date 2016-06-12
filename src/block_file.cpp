#pragma warning(disable:4996)
#include "block_file.h"
#include <cstring>

// -----------------------------------------------------------------------------
//  Some points to NOTE:
//  1) 2 types of block # are used (i.e. the internal # (e.g. act_block) 
//     and external # (e.g. pos)). internal # is one larger than external #
//     because the first block of the file is used to store header info. 
//     data info is stored starting from the 2nd block (excluding the header
//     block).both types of # start from 0.
//
//  2) "act_block" is internal block #. "number" is the # of data block (i.e. 
//     excluding the header block). maximum actblock equals to number. Maximum 
//     external block # equals to number - 1 
//
//  3) cache_cont records the internal numbers. 
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//  BlockFile: structure of reading and writing file for b-tree
// -----------------------------------------------------------------------------
BlockFile::BlockFile(				// constructor
	char* name,							// file name
	int b_length)						// block length
{
	file_name_ = new char[strlen(name) + 1];
	strcpy(file_name_, name);
	block_length_ = b_length;

	num_blocks_ = 0;				// num of blocks, init to 0
	// -------------------------------------------------------------------------
	//  Init <fp> and open <file_name_>. If <file_name_> exists, then fp != 0,
	//  and we excute if-clause program. Otherwise, we excute else-clause 
	//  program.
	//
	//  "rb+": read or write data from or into binary doc. if the file not 
	//  exist, it will return nullptr.
	//  用二进制格式打开文件准备读写
	// -------------------------------------------------------------------------
	if ((fp_ = fopen(name, "rb+")) != nullptr) {
		// ---------------------------------------------------------------------
		//  Init <new_flag_> (since the file exists, <new_flag_> is false).
		//  Reinit <block_length_> (determined by the doc itself).
		//  Reinit <num_blocks_> (number of blocks in doc itself).
		// ---------------------------------------------------------------------
		new_flag_ = false;			// reinit <block_length_> by file
		block_length_ = fread_number();
		num_blocks_ = fread_number();
	} else {
		// ---------------------------------------------------------------------
		//  <file_name_> not exists. we construct new file and reinit paras.
		// ---------------------------------------------------------------------
		if (block_length_ < BFHEAD_LENGTH) {
			// -----------------------------------------------------------------
			//  Ensure <block_length_> is larger than or equal to 8 bytes.
			//  8 bytes = 4 bypes <block_length_> + 4 bytes <num_blocks_>.
			//  意思是，由于header部分储存了2个int，它至少有8 bytes的大小
			//  block_length_比这还小，是不合理的。
			// -----------------------------------------------------------------
			error("BlockFile::BlockFile couldnot open file.\n", true);
		}

		// ---------------------------------------------------------------------
		//  "wb+": read or write data from or into binary doc. if file not
		//  exist, we will construct a new file.
		// ---------------------------------------------------------------------
		fp_ = fopen(file_name_, "wb+");
		if (fp_ == nullptr) {
			error("BlockFile::BlockFile could not create file.\n", true);
		}

		// ---------------------------------------------------------------------
		//  Init <new_flag_>: as file is just constructed (new), it is true.
		//  Write <block_length_> and <num_blocks_> to the header of file.
		//  Since the file is empty (new), <num_blocks_> is 0 (no blocks in it).
		// ---------------------------------------------------------------------
		new_flag_ = true;
		fwrite_number(block_length_);
		fwrite_number(0);

		// ---------------------------------------------------------------------
		//  Since <block_length_> >= 8 bytes, for the remain bytes, we will 
		//  init 0 to them.
		//  用0填充一个block剩下的部分。
		//  ftell() 返回从文件开头到目前的位置有几个bytes。
		// ---------------------------------------------------------------------
		char* buffer = nullptr;
		int len = block_length_ - static_cast<int>(ftell(fp_));				// cmpt remain length of a block
		buffer = new char[len];
									// set to 0 to remain bytes
		memset(buffer, 0, sizeof(buffer));
		put_bytes(buffer, len);

		delete[] buffer;
		buffer = nullptr;
	}
	// -------------------------------------------------------------------------
	//  Redirect file pointer to the start position of the file
	// -------------------------------------------------------------------------
	fseek(fp_, 0, SEEK_SET);
	act_block_ = 0;					// init <act_block_> (no blocks)
}

// -----------------------------------------------------------------------------
BlockFile::~BlockFile()				// destructor
{
	if (file_name_) {				// release space of <file_name_>
		delete[] file_name_;
		file_name_ = nullptr;
	}
	if (fp_) fclose(fp_);			// close <fp_>
}

// -----------------------------------------------------------------------------
void BlockFile::fwrite_number(		// write an <int> value to bin file
	int value) const  // a value of type <int>
{
	put_bytes(reinterpret_cast<char *>(&value), SIZEINT);
}

// -----------------------------------------------------------------------------
int BlockFile::fread_number() const  // read an <int> value from bin file
{
	char ca[SIZEINT];
	get_bytes(ca, SIZEINT);

	return *reinterpret_cast<int *>(ca);
}

// -----------------------------------------------------------------------------
//  Note that this func does not read the header of blockfile. It fetches the 
//  info in the first block excluding the header of blockfile.
//  用来跳过header，为读取后面的数据做好准备。
// -----------------------------------------------------------------------------
void BlockFile::read_header(		// read remain bytes excluding header
	char* buffer)						// contain remain bytes (return)
{
									// jump out of first 8 bytes
	fseek(fp_, BFHEAD_LENGTH, SEEK_SET);
									// read remain bytes into <buffer>
	get_bytes(buffer, block_length_ - BFHEAD_LENGTH); 

	if (num_blocks_ < 1) {			// no remain bytes
		fseek(fp_, 0, SEEK_SET);	// fp return to beginning pos
		act_block_ = 0;				// no act block
	} else {
		// ---------------------------------------------------------------------
		//  Since we have read the first block (header block) of block file,
		//  thus <act_block_> = 1, and the file pointer point to the 2nd block
		//  (first block to store real data).
		// ---------------------------------------------------------------------
		act_block_ = 1;
	}
}

// -----------------------------------------------------------------------------
//  Note that this func does not write the header of blockfile. It writes the 
//  info in the first block excluding the header of blockfile.
//  在基本的header（2个int，分别为block的长度和个数）以外储存额外的信息。
// -----------------------------------------------------------------------------
void BlockFile::set_header(			// set remain bytes excluding header
	char* header)					// contain remain bytes
{
									// jump out of first 8 bytes
	fseek(fp_, BFHEAD_LENGTH, SEEK_SET);
									// write remain bytes into <buffer>
	put_bytes(header, block_length_ - BFHEAD_LENGTH);
	
	if (num_blocks_ < 1) {			// no remain bytes
		fseek(fp_, 0, SEEK_SET);	// fp return to beginning pos
		act_block_ = 0;				// no act block
	}
	else {
		// ---------------------------------------------------------------------
		//  Since we have write the first block (header block) of block file,
		//  thus <act_block_> = 1, and the file pointer point to the 2nd block 
		//  (first block to store real data).
		// ---------------------------------------------------------------------
		act_block_ = 1;
	}
}

// -----------------------------------------------------------------------------
//  Read a <block> from <index>
//
//  We point out the difference of counting among the <number>, <act_block> 
//  and <pos>.
//  (1) <num_blocks_>: 记录block的个数，不包括header的block，从1开始。
//  (2) <act_block_>: 记录当前已经读写的block的个数，包括header的block，
//      因此当我们在读写文件的时候，<act_block>为1（已经读过了header）。它和文件指针相对应。 
//  (3) <index> : 记录我们打算读写的block的位置，不包括header的block，从0开始。
//      例如当<index> = 0时，文件指针指向header的block的下一个block，此时<act_block_>等于1。
//
//  例子：如果number为3，那么文件里一共有4个block，其中有一个header的block和3个data的block。
//
//  当文件被打开，<act_block_>为1。如果<index>为1，这意味着我们打算读写第三个block
//  （第二个data的block），因此首先index自增变成2，然后feesk把文件指针移到第二个data的block。
//
//  对第二个data的block读写结束后，文件指针指向第三个数据的block。此时已经读写了3个block，
//  （1个header，2个data）因此现在<act_block> = <index> + 1 = 2 + 1 = 3。
// -----------------------------------------------------------------------------
bool BlockFile::read_block(			// read a <block> from <index>
	Block block,						// a <block> (return)
	int index)							// 要读取第几号data的block（从0开始）
{
	index++;						// 自增后，表示要所有block中的第几号（header的block是第0号）
									// move to the position
	if (index <= num_blocks_ && index > 0) {
		seek_block(index);
		// 举个例子，假设此时index是1，代表要访问第一个data的block，
		// 如果此时act_block_和index相等，也为1，代表已经读写过1个块（就是header块）。
		// 代表此时文件指针指向第一个data的block的开头，因此文件指针不需要移动。
		// 查看seek_block函数的源码来进一步了解。
	}
	else {
		printf("BlockFile::read_block request the block %d "
			"which is illegal.", index - 1);
		error("\n", true);
	}

	get_bytes(block, block_length_);// read the block
	if (index + 1 > num_blocks_) {	// <fp_> reaches the end of file
		fseek(fp_, 0, SEEK_SET);
		act_block_ = 0;				// <act_block_> rewind to start pos
	}
	else {
		act_block_ = index + 1;		// <act_block_> to next pos
	}
	return true;
}

// -----------------------------------------------------------------------------
//  这个函数是用来写已经存在的block的，不能超过<num_blocks>的范围。
//  如果要分配新的block，应该调用append_block函数。
// -----------------------------------------------------------------------------
bool BlockFile::write_block(		// write a <block> into <index>
	Block block,						// a <block>
	int index)							// position of the blocks
{
	index++;						// extrnl block to intrnl block
									// move to the position
	if (index <= num_blocks_ && index > 0) {
		seek_block(index);
	}
	else {
		printf("BlockFile::write_block request the block %d "
			"which is illegal.", index - 1);
		error("\n", true);
	}
	
	put_bytes(block, block_length_);// write this block
	if (index + 1 > num_blocks_) {	// update <act_block_>
		fseek(fp_, 0, SEEK_SET);
		act_block_ = 0;
	}
	else {
		act_block_ = index + 1;
	}
	return true;
}

// -----------------------------------------------------------------------------
//  在文件末尾追加新的block。文件指针指向新追加的block并返回它的位置(从0开始的不包括header的block编号)。
// -----------------------------------------------------------------------------
int BlockFile::append_block(		// append new block at the end of file
	Block block)						// the new block
{
	fseek(fp_, 0, SEEK_END);		// <fp_> point to the end of file
	put_bytes(block, block_length_);// write a <block>
	num_blocks_++;					// add 1 to <num_blocks_>
	
	fseek(fp_, SIZEINT, SEEK_SET);	// <fp_> point to pos of header，跳过一个int的大小，因为第一个int储存一个block的大小
	fwrite_number(num_blocks_);		// update <num_blocks_>

	// -------------------------------------------------------------------------
	//  <fp_> point to the pos of new added block. 
	//  The equation <act_block_> = <num_blocks_> indicates the file pointer 
	//  point to new added block.
	//  Return index of new added block
	// -------------------------------------------------------------------------
	fseek(fp_, -block_length_, SEEK_END);
	act_block_ = num_blocks_;
	return act_block_ - 1;
}

// -----------------------------------------------------------------------------
//  Delete last <num> block in the file.
//
//  只是修改了block的个数，数据还是储存在文件中，文件大小不变。
// -----------------------------------------------------------------------------
bool BlockFile::delete_last_blocks(	// delete last <num> blocks
	int num)							// number of blocks to be deleted
{
	if (num > num_blocks_) {		// check whether illegal?
		return false;
	}

	num_blocks_ -= num;				// update <number>
	fseek(fp_, SIZEINT, SEEK_SET);
	fwrite_number(num_blocks_);

	fseek(fp_, 0, SEEK_SET);		// <fp> point to beginning of file
	act_block_ = 0;					// <act_block> = 0
	return true;
}

