#include "mab.h"

// 2^13 is the lowest power of 2 that can hold 8mb
// Used to determine max memory for a single process
#define MIN_ALLOC_LOG2 13
#define MIN_ALLOC (1 << MIN_ALLOC_LOG2)

// 2^31 is the lowest power of 2 that can hold 2gb 
// Used to determine size of the total memory pool
#define MAX_ALLOC_LOG2 31
#define MAX_ALLOC (1 << MIN_ALLOC_LOG2)

// Maximum number of possible nodes
#define MAX_BLOCKS ((MAX_ALLOC_LOG2 - MIN_ALLOC_LOG2) * 2)

// Starting address for this allocator's memory range.
// Offsets start with this as 0
static uint8_t *base_ptr;

// Maximum address used by the allocator.
// Used to know when to request more memory from kernel.
static uint8_t *max_ptr;


struct Stack {
    MabPtr mab_ptr;
    struct Stack * next;  
} *top;


Stack stack_pop() {

    if (top == NULL) {

        return NULL;

    }

    Stack tmp = top;
    MabPtr m = tmp->mab_ptr;
    free(top);
    top = tmp->next;

    return m;

}


void stack_push(MabPtr m) {

    struct Stack * new_stack_item = (struct Stack *) malloc(1 * sizeof(struct Stack));

    if (!new_stack_item) {

        fprintf(stderr, "ERROR: Could not create new stack item\n");
        return NULL;

    }

    new_stack_item->mab_ptr = m;
    new_stack_item->next = NULL;

    if (top != NULL) {

        new_stack_item->next = top;

    }
    
    top = new_stack_item;

}


MabPtr findBlock(MabPtr m, int size) {

    MabPtr tmp = m;

    

    // Create left child if needed
    if (tmp->left_child == NULL) {

        if ( !(new_left_child = (MabPtr) malloc(sizeof(Mab))) ) {

            fprintf(stderr, "ERROR: Could not create new memory access block\n");
            return NULL;

        }
        
        new_left_child->offset = ;
        new_left_child->size = allocation;
        new_left_child->allocated = 0;
        new_left_child->parent = tmp;
        new_left_child->left_child = NULL;
        new_left_child->right_child = NULL;

        tmp->left_child = new_left_child;

    }
    
    // Create right child if needed
    if (tmp->right_child == NULL) {

        if ( !(new_right_child = (MabPtr) malloc(sizeof(Mab))) ) {
            fprintf(stderr, "ERROR: Could not create new memory access block\n");
            return NULL;
        }
        
        new_right_child->offset = new_right_child;
        new_right_child->size = allocation;
        new_right_child->allocated = 0;
        new_right_child->parent = tmp;
        new_right_child->left_child = NULL;
        new_right_child->right_child = NULL;

        tmp->right_child = new_right_child;

    }

}

MabPtr memMerge(MabPtr m) { // merge buddy memory blocks

    //

    return 0;

}

MabPtr memSplit(MabPtr m, int size) { // split a memory block

    //

    return 0;

}

MabPtr memAlloc(MabPtr m, int size) { // allocate memory block

    // Check if request is possible
    if (size > MAX_ALLOC) {
        return NULL;
    }

    // Default to the minimum size if request is too small
    if (size < MIN_ALLOC) {
        size = MIN_ALLOC;
    }

    // If m is null we need to reserve the memory and initialise the tree
    if (m == NULL) {

        // Reserve the simulated memory space
        char* start_Ptr = malloc(MAX_ALLOC);
        
        if (start_Ptr == NULL) {
            fprintf(stderr, "ERROR: Could not initialise memory access tree\n");
            return NULL;
        }

        // Initialise the tree
        if ( !(m = (MabPtr) malloc(sizeof(Mab))) ) {
            fprintf(stderr, "ERROR: Could not create new memory access block\n");
            return NULL;
        }
        
        m->offset = start_Ptr;
        m->size = MAX_ALLOC;
        m->allocated = 0;
        m->parent = NULL;
        m->left_child = NULL;
        m->right_child = NULL;

    }

    // Find a block for this memory request
    MabPtr tmp = m;
    int allocation = MAX_ALLOC;

    while (allocation > size) {

        // Scan the powers of 2
        allocation = allocation / 2;
        
        // Create left child if needed
        if (tmp->left_child == NULL) {

            if ( !(new_left_child = (MabPtr) malloc(sizeof(Mab))) ) {
                fprintf(stderr, "ERROR: Could not create new memory access block\n");
                return NULL;
            }
            
            new_left_child->offset = ;
            new_left_child->size = allocation;
            new_left_child->allocated = 0;
            new_left_child->parent = tmp;
            new_left_child->left_child = NULL;
            new_left_child->right_child = NULL;

            tmp->left_child = new_left_child;

        }
        
        // Create right child if needed
        if (tmp->right_child == NULL) {

            if ( !(new_right_child = (MabPtr) malloc(sizeof(Mab))) ) {
                fprintf(stderr, "ERROR: Could not create new memory access block\n");
                return NULL;
            }
            
            new_right_child->offset = new_right_child;
            new_right_child->size = allocation;
            new_right_child->allocated = 0;
            new_right_child->parent = tmp;
            new_right_child->left_child = NULL;
            new_right_child->right_child = NULL;

            tmp->right_child = new_right_child;

        }

        // If the left child is not allocated
        if (!tmp->left_child->allocated) {
            
            // set the left child as tmp
            tmp = tmp->left_child;

        }
        
        // If the right child is not allocated
        else if (!tmp->left_child->allocated) {
            
            // set the right child as tmp
            tmp = tmp->left_child;

        // If both are already allocated
        } else {

            return NULL;

        }

    }



    return m;

}

MabPtr memFree(MabPtr m) { // free memory block

    //

    return 0;

}