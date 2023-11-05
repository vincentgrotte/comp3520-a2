#ifndef MAB
#define MAB

/* Include files */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

struct mab {
    int offset; // starting address of the memory block
    int size; // size of the memory block
    int allocated; // the block allocated or not
    struct mab * parent; // for use in the Buddy binary tree
    struct mab * left_child; // for use in the binary tree
    struct mab * right_child; // for use in the binary tree
};

typedef struct mab Mab;
typedef Mab * MabPtr;

MabPtr memMerge(MabPtr m); // merge buddy memory blocks
MabPtr memSplit(MabPtr m, int size); // split a memory block
MabPtr memAlloc(MabPtr m, int size); // allocate memory block
MabPtr memFree(MabPtr m); // free memory block

// 2^13 is the lowest power of 2 of bytes that can hold 8mb
// Used to determine max memory for a single process
// 8192
static unsigned int MIN_MEM = 8;

// 2^31 is the lowest power of 2 of bytes that can hold 2gb 
// Used to determine size of the total memory pool
// 2147483648
static unsigned int MAX_MEM = 2048;

typedef struct mabStack {
    MabPtr mab_ptr;
    struct mabStack * next;  
} MabStack;

#endif