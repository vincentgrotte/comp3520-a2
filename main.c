#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// sleep import by platform
// #include <windows.h>
#include <unistd.h>

// submodules
#include "some-routine.h"
#include "test-params.h"

// Parameters
int param_x; // param_x

// Global variables
int x_global = 0;

// Mutex
pthread_mutex_t x_mutex = PTHREAD_MUTEX_INITIALIZER;

// Cond
pthread_cond_t x_cond;

int main(int argc, char *argv[]) {

    pthread_t *threads; // system thread id
    int *t_ids; // user-defined thread id

    int k; // general-purpose counter
    int rc; // return code

    printf("\n");

    //
    // SET PARAMETERS VIA USER INPUT OR PRESET/TEST VALUES
    //

    // test values
    if ( argc > 1 ) {

        char *argument = (char*) &argv[1];
        char *flag = "-d"; // -d for development

        if ( strcmp(argument, flag) ) {
            printf("Using development configuration.\n");
            param_x = TEST_PARAM_X;
        } else {
            printf("Error: Bad Input. Exiting.\n");
            exit(-1);
        }

    // normal flow
    } else {
        printf("Please enter an integer for the total number of param_x:\n");
        scanf("%d", &param_x);
    }

    //
    // INITIALIZE CONDITION VARIABLES
    //

    printf("\n");

    // initialize condition variables
    rc = pthread_cond_init(&x_cond, NULL);
    if (rc) {
        printf("ERROR; return code from pthread_cond_init() is %d\n", rc);
        exit(-1);
    }

    //
    // INITIALIZE THREADS
    //

    int num_threads = param_x;
    
    threads = malloc((num_threads) * sizeof(pthread_t));
    if (threads == NULL) {
        fprintf(stderr, "threads out of memory\n");
        exit(1);
    }

    t_ids = malloc((num_threads) * sizeof(int));
    if (t_ids == NULL) {
        fprintf(stderr, "t_ids out of memory\n");
        exit(1);
    }

    //
    // EXECUTE THREADS
    //

    // threads
    for (k = 0; k < num_threads; k++) {
        t_ids[k] = k;
        rc = pthread_create(&threads[k], NULL, some_routine, (void *) &t_ids[k]);
        if (rc) {
            printf("ERROR; return code from pthread_create() (barber) is %d\n", rc);
            exit(-1);
        }
    }

    // wait for threads to finish
    for (k = 0; k < num_threads; k++) {
        pthread_join(threads[k], NULL);
    }

    //
    // CLEAN UP
    //

    // free memory
    free(threads);
    free(t_ids);

    // destroy mutex
    pthread_mutex_destroy(&x_mutex);

    // destroy cond
    pthread_cond_destroy(&x_cond);

    // exit threads
    printf("\nFinished.\n"); // log status: main-thread-finished

    return 0;
}
