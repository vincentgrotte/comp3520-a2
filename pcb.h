/* PCB include header file for RR dispatcher */

#ifndef PCB
#define PCB

/* Include files */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "mab.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* Process Management Definitions */
#define PCB_UNINITIALIZED 0
#define PCB_INITIALIZED 1
#define PCB_READY 2
#define PCB_RUNNING 3
#define PCB_SUSPENDED 4
#define PCB_TERMINATED 5

/* Custom Data Types */
struct pcb {
    pid_t pid;
    char * args[3];
    int arrival_time;
    int service_time;
    int cpu_time_spent;
    int cpu_time_spent_iteration;
    int iterations;
    int queue;
    int status;
    int size;
    MabPtr mab_ptr;
    struct pcb * next;
};

typedef struct pcb Pcb;
typedef Pcb * PcbPtr;

/* Function Prototypes */
PcbPtr StartPcb(PcbPtr);
PcbPtr SuspendPcb(PcbPtr);
PcbPtr TerminatePcb(PcbPtr);
PcbPtr PrintPcb(PcbPtr);
void   PrintPcbHdr(void);
PcbPtr CreatenullPcb();
PcbPtr EnqPcb(PcbPtr, PcbPtr);
PcbPtr DeqPcb(PcbPtr*);

#endif
