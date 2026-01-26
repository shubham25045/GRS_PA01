/*
 * MT25045_Part_A_Program_A.c
 * Program A: Process-based execution using fork()
 * Creates N child processes (default 2) to execute worker functions
 * 
 * Usage: ./program_a <cpu|mem|io> [num_processes]
 * 
 * Roll No: MT25045
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "MT25045_Part_B_workers.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <cpu|mem|io> [num_processes]\n", argv[0]);
        return 1;
    }

    const char *worker_type = argv[1];
    int num_processes = (argc >= 3) ? atoi(argv[2]) : 2;

    if (num_processes < 1) {
        fprintf(stderr, "Error: Number of processes must be at least 1\n");
        return 1;
    }

    /* Validate worker type */
    if (strcmp(worker_type, "cpu") != 0 && 
        strcmp(worker_type, "mem") != 0 && 
        strcmp(worker_type, "io") != 0) {
        fprintf(stderr, "Error: Unknown worker type '%s'\n", worker_type);
        return 1;
    }

    printf("[Parent PID: %d] Creating %d child process(es) for '%s' worker\n", 
           getpid(), num_processes, worker_type);

    /* Create child processes */
    for (int i = 0; i < num_processes; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            return 1;
        } 
        else if (pid == 0) {
            /* Child process */
            printf("[Child %d, PID: %d] Starting %s worker\n", 
                   i + 1, getpid(), worker_type);

            if (strcmp(worker_type, "cpu") == 0) {
                worker_cpu();
            } else if (strcmp(worker_type, "mem") == 0) {
                worker_mem();
            } else if (strcmp(worker_type, "io") == 0) {
                worker_io();
            }

            printf("[Child %d, PID: %d] Finished %s worker\n", 
                   i + 1, getpid(), worker_type);
            exit(0);
        }
        /* Parent continues to create more children */
    }

    /* Parent waits for all children to complete */
    for (int i = 0; i < num_processes; i++) {
        int status;
        pid_t finished_pid = wait(&status);
        printf("[Parent] Child PID %d finished with status %d\n", 
               finished_pid, WEXITSTATUS(status));
    }

    printf("[Parent] All child processes completed.\n");
    return 0;
}
