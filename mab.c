#include "mab.h"


MabPtr stack_pop(MabStack * top) {

    if (top == NULL) {

        return NULL;

    }

    MabStack * tmp = top;
    MabPtr m = tmp->mab_ptr;
    free(top);
    top = tmp->next;

    return m;

}


MabStack * stack_push(MabPtr m, MabStack * top) {

    MabStack * new_stack_item = (MabStack *) malloc(sizeof(MabStack));

    if (!new_stack_item) {

        fprintf(stderr, "ERROR: Could not create new stack item\n");
        return new_stack_item;

    }

    new_stack_item->mab_ptr = m;
    new_stack_item->next = NULL;

    if (top != NULL) {

        new_stack_item->next = top;

    }
    
    top = new_stack_item;
    return top;

}


MabPtr memMerge(MabPtr m) { // merge buddy memory blocks

    MabPtr current_block = m;

    while (current_block &&
        current_block->parent &&
        !current_block->parent->right_child &&
        !current_block->parent->right_child->allocated &&
        !current_block->parent->left_child &&
        !current_block->parent->left_child->allocated) {

        current_block = current_block->parent;

        // Free both children
        free(current_block->right_child);
        free(current_block->left_child);

    }

    return 0;

}


MabPtr memSplit(MabPtr m, int size) { // split a memory block

    // Set m to unallocated
    m->allocated = 0;

    // Create left child
    MabPtr new_left_child = (MabPtr) malloc(sizeof(Mab));

    if ( !new_left_child ) {
        fprintf(stderr, "ERROR: Could not create new memory access block\n");
        return NULL;
    }
    
    new_left_child->offset = m->offset;
    new_left_child->size = size;
    new_left_child->allocated = 0;
    new_left_child->parent = m;
    new_left_child->left_child = NULL;
    new_left_child->right_child = NULL;

    // Attach left child to m
    m->left_child = new_left_child;

    // TODO: delete me
    // printf("CREATE_LEFT_CHILD offset:%d size:%d\n", new_left_child->offset, new_left_child->size);

    // Create right child
    MabPtr new_right_child = (MabPtr) malloc(sizeof(Mab));

    if ( !new_right_child ) {
        fprintf(stderr, "ERROR: Could not create new memory access block\n");
        return NULL;
    }
    
    new_right_child->offset = m->offset + (m->size / 2);
    new_right_child->size = size;
    new_right_child->allocated = 0;
    new_right_child->parent = m;
    new_right_child->left_child = NULL;
    new_right_child->right_child = NULL;

    // Attach right child to m
    m->right_child = new_right_child;

    // TODO: delete me
    // printf("CREATE_RIGHT_CHILD offset:%d size:%d\n", new_right_child->offset, new_right_child->size);

    return m;

}


MabPtr memAlloc(MabPtr m, int size) { // allocate memory block

    // printf("MEM_ALLOC exists:%d size:%d\n", !!m, size);

    // Check if request is possible
    if (size > MAX_MEM) {

        // printf("MAX_MEM %d smaller than %d\n", (int) MAX_MEM, size);

        exit(EXIT_FAILURE);

    }

    // Default to the minimum size if request is too small
    if (size < MIN_MEM) {

        // printf("MIN_MEM %d greater than %d\n", (int) MIN_MEM, size);

        size = MIN_MEM;

    }

    // If m is null we need to initialise the tree
    if (!m) {

        // Initialise the tree
        if ( !(m = (MabPtr) malloc(sizeof(Mab))) ) {
            fprintf(stderr, "ERROR: Could not create new memory access block\n");
            return NULL;
        }
        
        m->offset = 0;
        m->size = MAX_MEM;
        m->allocated = 0;
        m->parent = NULL;
        m->left_child = NULL;
        m->right_child = NULL;

        // printf("INIT_TREE size:%d\n", m->size);

        return m;

    }

    // Figure out which depth the block should have
    int target_size = MAX_MEM;
    int target_depth = 1;

    // printf("FIND_TARGET_DEPTH size:%d size_t:%d depth_t:%d\n", size, target_size, target_depth);

    while (target_size > size) {
    
        // Scan the powers of 2
        // Find the smallest allocation large enough
        target_size = target_size / 2;
        target_depth++;

        // printf("FIND_TARGET_DEPTH size:%d size_t:%d depth_t:%d\n", size, target_size, target_depth);

    }

    // Find a block for this memory request
    MabPtr current_block = m;
    int current_depth = 1;
    MabStack * unvisited = (MabStack *) malloc(sizeof(MabStack));

    // printf("SCAN_AT_DEPTH depth:%d\n", current_depth);
    
    // While there are still blocks to visit
    while ( unvisited && current_block ) {

        // If the current block is unallocated and at the target depth
        if (!current_block->allocated && current_depth == target_depth) {
            
            current_block->allocated = 1;

            // printf("FOUND_ALLOC depth:%d\n", current_depth);

            if (unvisited) {

                // printf("UNVISITED STILL HAS BLOCKS FLOATING AROUND\n");

                // while (unvisited && unvisited->next) {

                //     MabStack * next = unvisited->next;
                //     unvisited = unvisited->next;
                //     free(unvisited);
                //     unvisited = next;

                // }

            }

            break;

        }

        // If there are no child blocks
        if (!current_block->right_child && !current_block->left_child) {

            // New blocks will be half the current size
            current_block = memSplit(current_block, ( current_block->size / 2 ));

            // printf("MEM_SPLIT size:%d depth:%d\n", current_block->size, current_depth);

        }

        // If the right child is not allocated
        if (!current_block->right_child->allocated) {
            
            unvisited = stack_push(current_block->right_child, unvisited);

            // printf("STACK_RIGHT_CHILD size:%d depth:%d\n", ( current_block->size / 2 ), current_depth);

        }

        // If the left child is not allocated
        if (!current_block->left_child->allocated) {
            
            unvisited = stack_push(current_block->left_child, unvisited);

            // printf("STACK_LEFT_CHILD size:%d depth:%d\n", ( current_block->size / 2 ), current_depth);

        }

        if (!current_block->right_child->allocated
            || !current_block->left_child->allocated) {

            current_depth++;

        } else {

            current_depth--;

        }

        // pop the next block on the unvisited stack into current_block
        current_block = stack_pop(unvisited);

        // printf("SCAN_AT_DEPTH depth:%d\n", current_depth);

    }

    // // clean up remaining unvisited blocks
    // // this can happen if the target block is found before all blocks are visited
    // if (unvisited) {

    //     while (unvisited) stack_pop(unvisited);
    
    // }

    return current_block;

}


MabPtr memFree(MabPtr m) { // free memory block

    m->allocated = 0;

    if (m->parent &&
        !m->parent->right_child->allocated &&
        !m->parent->left_child->allocated) {

        memMerge(m->parent);

    }

    return 0;

}
