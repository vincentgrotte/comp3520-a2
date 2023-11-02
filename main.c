#include "main.h"


int main(int argc, char *argv[]) {

    // Parameters
    int param_t0; // time quanta for Level-0 queue
    int param_t1; // time quanta for Level-1 queue
    int param_k; // number of iterations for a job in the Level-1 queue

    // ---------------------------
    //  Allow user to set parameters via command line
    // ---------------------------

    if ( argc == 5 ) {

        printf("Using command line params\n");

        sscanf(argv[2], "%d", &param_t0);
        sscanf(argv[3], "%d", &param_t1);
        sscanf(argv[4], "%d", &param_k);
            
        printf("L0Q max time: %d\n", param_t0);
        printf("L1Q max time: %d\n", param_t1);
        printf("L1Q max iterations: %d\n", param_k);

    // ---------------------------
    //  "Normal" Flow - Ask user for each parameter as console input
    // ---------------------------

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

    // ---------------------------
    //  Main function variable declarations
    // ---------------------------

    FILE * input_list_stream = NULL;
    PcbPtr job_queue = NULL;
    PcbPtr L0_queue = NULL;
    PcbPtr L1_queue = NULL;
    PcbPtr L2_queue = NULL;
    PcbPtr current_process = NULL;
    PcbPtr process = NULL;
    int timer = 0;

    int turnaround_time;
    double av_turnaround_time = 0.0, av_wait_time = 0.0;
    int n = 0;

    // ---------------------------
    //  Read job list from command line argument
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
    
    // ---------------------------
    //  Populate the job queue
    // ---------------------------

    printf("Loaded jobs from %s\n", argv[1]);

    while (!feof(input_list_stream)) {  // put processes into job_queue
        
        process = CreatenullPcb();
        
        if (fscanf(input_list_stream, "%d, %d\n",
            &(process->arrival_time),
            &(process->service_time)) != 2) {

            free(process);
            continue;
        
        }

        process->status = PCB_INITIALIZED;
        job_queue = EnqPcb(job_queue, process);
	    n++;

    }

    // ---------------------------
    //  Handle the queues
    // ---------------------------

    // While:
    // - there's anything in any of the queues
    // - or there is a currently running process:

    while ( job_queue || L0_queue || L1_queue || L2_queue || current_process ) {

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
                
                // Free up process structure memory
                free(current_process);
                current_process = NULL;

            }

            // If times up in L0 queue
            else if (current_process->queue == 0 &&
                current_process->cpu_time_spent >= param_t0) {

                // Send SIGTSTP to suspend it
                SuspendPcb(current_process);

                // Enqueue it on the L1 queue
                current_process->queue = 1;
                L1_queue = EnqPcb(L1_queue, current_process);
                current_process = NULL;                   

            }

            // If process in L1 queue has used up all its iterations
            else if (current_process->queue == 1 &&
                current_process->iterations >= param_k) {
            
                // Send SIGTSTP to suspend it
                SuspendPcb(current_process);

                // Enqueue it on the L2 queue
                current_process->queue = 2;
                L2_queue = EnqPcb(L2_queue, current_process);
                current_process = NULL;

            }

            // If times up in L1 queue for this iteration
            else if (current_process->queue == 1 &&
                current_process->cpu_time_spent_iteration >= param_t1) {
                
                // Increment process iterations and reset iteration timer
                current_process->iterations++;
                current_process->cpu_time_spent_iteration = 0;

                // Send SIGTSTP to suspend it
                SuspendPcb(current_process);
                
                // Enqueue it on the end of the L1 queue
                L1_queue = EnqPcb(L1_queue, current_process);
                current_process = NULL;                
                
            }

            else if (current_process->queue == 2) {
            
                // If anything is waiting in an other queue, suspend this process 
                if (L0_queue || L1_queue) {

                    // Send SIGTSTP to suspend it
                    SuspendPcb(current_process);

                    // Enqueue it on the L2 queue
                    L2_queue = EnqPcb(L2_queue, current_process);
                    current_process = NULL;                    

                }
            
            }

            // Increment process cpu time spent;
            if (current_process) {
                current_process->cpu_time_spent++;
                current_process->cpu_time_spent_iteration++;
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

                // Dequeue process from L1 queue
                current_process = DeqPcb(&L2_queue);
                
                // (re)start it (send SIGCONT to it)
                StartPcb(current_process);

            } else {

                //

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
