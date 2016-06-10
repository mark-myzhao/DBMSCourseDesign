#ifndef _DEF_H_
#define _DEF_H_

#include <cstdlib>

// -----------------------------------------------------------------------------
//  Typedefs
// -----------------------------------------------------------------------------
typedef char Block[];  // 意思是，Block类型=char[]类型
typedef int POSPOINTER;           //  distinguish int from position pointers

// -----------------------------------------------------------------------------
//  Macros
// -----------------------------------------------------------------------------
#define MIN(a, b)	(((a) < (b)) ? (a) : (b))
#define MAX(a, b)	(((a) > (b)) ? (a) : (b))

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

// -----------------------------------------------------------------------------
//  Constants
// -----------------------------------------------------------------------------
const float E = 2.7182818F;		// math constants
const float PI = 3.141592654F;

const float FLOATZERO = 1e-6F;		// accuracy
									// max real value
const float MAXREAL = 3.402823466e+38F;
const float MINREAL = -MAXREAL;	// min real value

const long  MAXMEMORY = 1073741824;	// max memory, 1 GB
const int   MAXINT = 2147483647;	// max integer value
const int   MININT = -MAXINT;	// min integer value

								// size of type <int>
const int   SIZEINT = (int) sizeof(int);
// size of type <char>
const int   SIZECHAR = (int) sizeof(char);
// size of type <float>
const int   SIZEFLOAT = (int) sizeof(float);
// size of type <bool>
const int   SIZEBOOL = (int) sizeof(bool);

// file header size
const int   BFHEAD_LENGTH = (int)(sizeof(int) * 2);
// index size of leaf node
const int   INDEX_SIZE_LEAF_NODE = 4096;
const int   MAXK = 100;				// max top-k value

const int BUFFERSIZE = 10000;    //  10KB
const BLOKCLENGTH = 1000;        //  1KB
const int NODESIZE = 40;         //  40 entries in one node
// -----------------------------------------------------------------------------
//  Funuctions
// -----------------------------------------------------------------------------
inline void error(							// display an error message
	char* msg,							// message
	bool is_exit)						// whether exit
{
	printf(msg);
	if (is_exit) exit(1);
}

#endif