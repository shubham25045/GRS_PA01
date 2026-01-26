/*
 * MT25045_Part_A_Program_B.c
 * Program B: Thread-based execution using pthread
 * Creates N threads (default 2) to execute worker functions
 * 
 * Usage: ./program_b <cpu|mem|io> [num_threads]
 * 
 * Roll No: MT25045
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "MT25045_Part_B_workers.h"

/* Thread argument structure */
typedef struct {
    int thread_id;
    const char *worker_type;
} thread_arg_t;

/* Thread function wrapper */
void *thread_worker(void *arg) {
    thread_arg_t *targ = (thread_arg_t *)arg;
    
    printf("[Thread %d, TID: %lu] Starting %s worker\n", 
           targ->thread_id, pthread_self(), targ->worker_type);

    if (strcmp(targ->worker_type, "cpu") == 0) {
        worker_cpu();
    } else if (strcmp(targ->worker_type, "mem") == 0) {
        worker_mem();
    } else if (strcmp(targ->worker_type, "io") == 0) {
        worker_io();
    }

    printf("[Thread %d, TID: %lu] Finished %s worker\n", 
           targ->thread_id, pthread_self(), targ->worker_type);

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <cpu|mem|io> [num_threads]\n", argv[0]);
        return 1;
    }

    const char *worker_type = argv[1];
    int num_threads = (argc >= 3) ? atoi(argv[2]) : 2;

    if (num_threads < 1) {
        fprintf(stderr, "Error: Number of threads must be at least 1\n");
        return 1;
    }

    /* Validate worker type */
    if (strcmp(worker_type, "cpu") != 0 && 
        strcmp(worker_type, "mem") != 0 && 
        strcmp(worker_type, "io") != 0) {
        fprintf(stderr, "Error: Unknown worker type '%s'\n", worker_type);
        return 1;
    }

    printf("[Main] Creating %d thread(s) for '%s' worker\n", 
           num_threads, worker_type);

    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    thread_arg_t *args = malloc(num_threads * sizeof(thread_arg_t));

    if (!threads || !args) {
        perror("malloc failed");
        return 1;
    }

    /* Create threads */
    for (int i = 0; i < num_threads; i++) {
        args[i].thread_id = i + 1;
        args[i].worker_type = worker_type;

        int ret = pthread_create(&threads[i], NULL, thread_worker, &args[i]);
        if (ret != 0) {
            fprintf(stderr, "pthread_create failed for thread %d\n", i + 1);
            return 1;
        }
    }

    /* Wait for all threads to complete */
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        printf("[Main] Thread %d joined\n", i + 1);
    }

    free(threads);
    free(args);

    printf("[Main] All threads completed.\n");
    return 0;
}

