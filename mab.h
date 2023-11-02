struct mab {
 int offset; //starting address of the memory block
 int size; //size of the memory block
 int allocated; //the block allocated or not
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