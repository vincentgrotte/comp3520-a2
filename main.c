#include "main.h"

// Parameters
int param_k; // param_k
int param_t0; // param_t0
int param_t1; // param_t1

extern const int TEST_PARAM_K;
extern const int TEST_PARAM_T0;
extern const int TEST_PARAM_T1;


int main(int argc, char *argv[]) {

    printf("\n");

    // ---------------------------
    //  0. Set parameters via user input or with preset/test values
    // ---------------------------

    // test values
    if ( argc > 2 ) {

        char *argument = (char*) &argv[2];
        char *flag = "-t"; // -t for test

        if ( strcmp(argument, flag) ) {

            printf("Using test configuration.\n");
            param_k = TEST_PARAM_K;
            param_t0 = TEST_PARAM_T0;
            param_t1 = TEST_PARAM_T1;

        } else {

            printf("Error: Bad Input. Exiting.\n");
            exit(-1);

        }

    // normal flow
    } else {

        printf("Please enter an integer for the time quanta for Level-0 queue:\n");
        scanf("%d", &param_t0);

        if (param_t0 <= 0) {
        
            printf("Input must be greater than 0.\n");
            exit(EXIT_FAILURE);
        
        }

        printf("Please enter an integer for the time quanta for Level-1 queue\n");
        scanf("%d", &param_t1);

        if (param_t1 <= 0) {
        
            printf("Input must be greater than 0.\n");
            exit(EXIT_FAILURE);
        
        }

        printf("Please enter an integer for the number of iterations for a job in the Level-1 queue:\n");
        scanf("%d", &param_k);

        if (param_k <= 0) {
        
            printf("Input must be greater than 0.\n");
            exit(EXIT_FAILURE);
        
        }

    }

    /*** Main function variable declarations ***/

    FILE * input_list_stream = NULL;
    PcbPtr job_queue = NULL;
    PcbPtr L0_queue = NULL;
    PcbPtr L1_queue = NULL;
    PcbPtr L2_queue = NULL;
    PcbPtr current_process = NULL;
    PcbPtr process = NULL;
    int timer = 0;
    int time_quantum;
    int quantum;

    int turnaround_time;
    double av_turnaround_time = 0.0, av_wait_time = 0.0;
    int n = 0;

    // ---------------------------
    //  Check input
    // ---------------------------

    if (argc <= 0) {

        fprintf(stderr, "FATAL: Bad arguments array\n");
        exit(EXIT_FAILURE);

    }

    else if (argc < 2) {

        fprintf(stderr, "Usage: %s <TESTFILE>\n", argv[0]);
        exit(EXIT_FAILURE);

    }

    if (!(input_list_stream = fopen(argv[1], "r"))) {

        fprintf(stderr, "ERROR: Could not open \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);

    }
    
    printf("Level-0 max cpu time: %d\n", param_t0);
    printf("Level-1 max cpu time: %d\n", param_t1);
    printf("Level-1 queue max iterations: %d\n", param_k);

    // ---------------------------
    //  Populate the job queue
    // ---------------------------

    printf("Reading jobs from %s\n", argv[1]);

    while (!feof(input_list_stream)) {  // put processes into job_queue
        
        process = CreatenullPcb();
        
        if (fscanf(input_list_stream, "%d, %d\n",
            &(process->arrival_time),
            &(process->service_time)) != 2) {

            free(process);
            continue;
        
        }

        printf("%d, %d\n", process->arrival_time, process->service_time);
        process->status = PCB_INITIALIZED;
        job_queue = EnqPcb(job_queue, process);
	    n++;

    }

    printf("Finished loading job queue\n");

    // ---------------------------
    //  Handle the queues
    // ---------------------------

    // While:
    // - there's anything in any of the queues
    // - or there is a currently running process:

    while ( job_queue || L0_queue || L1_queue || current_process ) {
    // while (job_queue || L0_queue || L1_queue || L2_queue || current_process ) {

        // Unload any pending processes from the input queue:
        // While (head-of-input-queue.arrival-time <= dispatcher timer)
        // dequeue process from input queue and enqueue on L0 queue;
        while (job_queue && job_queue->arrival_time <= timer) {

            process = DeqPcb(&job_queue);             // dequeue process
            process->status = PCB_READY;                // set pcb ready
            L0_queue = EnqPcb(L0_queue, process);     // & put on L0 queue

        }

        // If a process is currently running;
        if (current_process) {
            
            // If the process is complete
            if (current_process->cpu_time_spent >= current_process->service_time) {

                // Send SIGINT to the process to terminate it
                TerminatePcb(current_process);

                // Calculate and accumulate turnaround time and wait time;
                turnaround_time = timer - current_process->arrival_time;
                av_turnaround_time += turnaround_time;
                av_wait_time += (turnaround_time - current_process->service_time);   
                
                printf(
                    "[L0::%d] turnaround time = %d, waiting time = %d\n",
                    current_process->pid,
                    turnaround_time, 
                    turnaround_time - current_process->service_time
                );

                // Free up process structure memory
                free(current_process);
                current_process = NULL;

            }

            // If times up in L0 queue
            else if (current_process->queue == 0 &&
                current_process->cpu_time_spent >= param_t0) {

                // Send SIGTSTP to suspend it
                SuspendPcb(current_process);

                // Calculate and accumulate turnaround time and wait time;
                turnaround_time = timer - current_process->arrival_time;
                av_turnaround_time += turnaround_time;
                av_wait_time += (turnaround_time - current_process->service_time);   
                
                // Reset cpu_time_spent
                current_process->cpu_time_spent = 0;

                printf("[L0::%d] rolling over to L1 \n", current_process->pid);

                // Enqueue it on the L1 queue
                current_process->queue = 1;
                L1_queue = EnqPcb(L1_queue, current_process);
                current_process = NULL;                   

            }

            // If process in L1 queue has used up all its iterations
            else if (current_process->queue == 1 &&
                current_process->iterations >= param_k) {
            
                // Send SIGTSTP to suspend it
                // SuspendPcb(current_process);
                TerminatePcb(current_process);

                // Calculate and accumulate turnaround time and wait time;
                turnaround_time = timer - current_process->arrival_time;
                av_turnaround_time += turnaround_time;
                av_wait_time += (turnaround_time - current_process->service_time);   
                
                // Reset cpu_time_spent
                current_process->cpu_time_spent = 0;

                printf("[L1::%d] rolling over to L2 \n", current_process->pid);

                // Enqueue it on the L2 queue
                current_process->queue = 2;
                L2_queue = EnqPcb(L2_queue, current_process);
                current_process = NULL;

                free(current_process);
                current_process = NULL;

            }

            // If times up in L1 queue for this iteration
            else if (current_process->queue == 1 &&
                current_process->cpu_time_spent >= param_t1) {
                
                // Increment process iterations
                current_process->iterations++;

                // Send SIGTSTP to suspend it
                SuspendPcb(current_process);

                // Calculate and accumulate turnaround time and wait time;
                turnaround_time = timer - current_process->arrival_time;
                av_turnaround_time += turnaround_time;
                av_wait_time += (turnaround_time - current_process->service_time);
                
                // Reset cpu_time_spent
                current_process->cpu_time_spent = 0;
                
                printf(
                    "[L1::%d] iteration (%d) finished\n",
                    current_process->pid,
                    current_process->iterations
                );

                // Enqueue it on the end of the L1 queue
                L1_queue = EnqPcb(L1_queue, current_process);
                current_process = NULL;                
                
            }

            else if (current_process->queue == 2) {
            
                // L2 stuff - Fcfs BUT INTERRUPTABLE
            
            }

            // Increment process cpu time spent;
            if (current_process) {
                current_process->cpu_time_spent++;
            }

        }

        // If no process currently running
        if (!current_process) {

            // IF L0 queue is not empty (1st priority)
            if (L0_queue) {

                // Dequeue process from L0 queue
                current_process = DeqPcb(&L0_queue);
                
                // If already started but suspended, restart it (send SIGCONT to it)
                // else start it (fork & exec)
                // Set it as currently running process;
                StartPcb(current_process);

            // IF L1 queue is not empty (2nd priority)
            } else if (L1_queue) {

                // Dequeue process from L1 queue
                current_process = DeqPcb(&L1_queue);
                
                // (re)start it (send SIGCONT to it)
                StartPcb(current_process);

            // IF L2 queue is not empty (lowest priority)
            } else if (L2_queue) {

                // Fcfs BUT INTERRUPTABLE

            } else {

                // Do nothing

            }

        }

        // Let the dispatcher sleep for one second
        sleep(1);
            
        // Increment dispatcher timer;
        timer++;
        
    }

    // print out average turnaround time and average wait time
    av_turnaround_time = av_turnaround_time / n;
    av_wait_time = av_wait_time / n;
    printf("average turnaround time = %f\n", av_turnaround_time);
    printf("average wait time = %f\n", av_wait_time);
    
    // 4. Terminate the dispatcher
    exit(EXIT_SUCCESS);
    
}
