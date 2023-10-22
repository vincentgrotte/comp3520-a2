#include <stdio.h>


void *some_routine(void *arg) {
    
    int routine_id = *((int *) arg) + 1;

    printf("[routine-id] %d\n", routine_id);

    return 0;

}

