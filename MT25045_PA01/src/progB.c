#include "workers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

// ---- Struct to pass arguments to each thread ----
typedef struct {
    const char *worker;     // "cpu" | "mem" | "io"
    int index;              // thread number (0..N-1)
    size_t mem_mb;          // for mem worker
    size_t file_mb;         // for io worker
    char filepath[256];     // per-thread filepath for io
} ThreadArgs;

// Print correct usage and exit
static void usage(const char *prog) {
    fprintf(stderr,
            "Usage:\n"
            "  %s <num_threads> cpu\n"
            "  %s <num_threads> mem <mem_mb>\n"
            "  %s <num_threads> io <file_mb> <filepath_prefix>\n\n"
            "Examples:\n"
            "  %s 4 cpu\n"
            "  %s 4 mem 256\n"
            "  %s 4 io 8 /tmp/pa01_io_thread\n",
            prog, prog, prog, prog, prog, prog);
    exit(1);
}

// Worker runner for threads (pthread requires: void* (*)(void*))
static void *thread_main(void *arg) {
    ThreadArgs *a = (ThreadArgs *)arg;

    if (strcmp(a->worker, "cpu") == 0) {
        worker_cpu(BASE_COUNT);
    } else if (strcmp(a->worker, "mem") == 0) {
        worker_mem(BASE_COUNT, a->mem_mb);
    } else if (strcmp(a->worker, "io") == 0) {
        worker_io(BASE_COUNT, a->filepath, a->file_mb);
    } else {
        fprintf(stderr, "Unknown worker in thread: %s\n", a->worker);
        return (void *)1;
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 3) usage(argv[0]);

    // Parse N (number of threads)
    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "num_threads must be > 0\n");
        return 1;
    }

    const char *worker = argv[2];

    // Defaults (used only for relevant workers)
    size_t mem_mb = 128;
    size_t file_mb = 32;

    // For IO: user provides a prefix, we generate prefix_<index>.bin
    const char *filepath_prefix = "/tmp/pa01_io_thread";

    // Parse extra args depending on worker type
    if (strcmp(worker, "cpu") == 0) {
        if (argc != 3) usage(argv[0]);

    } else if (strcmp(worker, "mem") == 0) {
        if (argc != 4) usage(argv[0]);
        mem_mb = (size_t)strtoul(argv[3], NULL, 10);
        if (mem_mb == 0) {
            fprintf(stderr, "mem_mb must be > 0\n");
            return 1;
        }

    } else if (strcmp(worker, "io") == 0) {
        if (argc != 5) usage(argv[0]);
        file_mb = (size_t)strtoul(argv[3], NULL, 10);
        filepath_prefix = argv[4];
        if (file_mb == 0 || filepath_prefix == NULL || filepath_prefix[0] == '\0') {
            fprintf(stderr, "file_mb must be > 0 and filepath_prefix must be valid\n");
            return 1;
        }

    } else {
        fprintf(stderr, "Unknown worker: %s\n", worker);
        usage(argv[0]);
    }

    // Allocate arrays for thread IDs and per-thread args
    pthread_t *tids = (pthread_t *)calloc((size_t)n, sizeof(pthread_t));
    ThreadArgs *args = (ThreadArgs *)calloc((size_t)n, sizeof(ThreadArgs));
    if (!tids || !args) {
        perror("calloc failed");
        free(tids);
        free(args);
        return 1;
    }

    // Create N threads
    for (int i = 0; i < n; i++) {
        args[i].worker = worker;
        args[i].index = i;
        args[i].mem_mb = mem_mb;
        args[i].file_mb = file_mb;

        // For IO worker: make per-thread file path
        if (strcmp(worker, "io") == 0) {
            // Example: /tmp/pa01_io_thread_0.bin
            snprintf(args[i].filepath, sizeof(args[i].filepath),
                     "%s_%d.bin", filepath_prefix, i);
        } else {
            // Not used for cpu/mem, but keep it valid
            args[i].filepath[0] = '\0';
        }

        int rc = pthread_create(&tids[i], NULL, thread_main, &args[i]);
        if (rc != 0) {
            fprintf(stderr, "pthread_create failed at i=%d (rc=%d)\n", i, rc);
            // Join already-created threads before exiting
            for (int j = 0; j < i; j++) {
                pthread_join(tids[j], NULL);
            }
            free(tids);
            free(args);
            return 1;
        }
    }

    // Join all threads
    int exit_code = 0;
    for (int i = 0; i < n; i++) {
        void *retval = NULL;
        int rc = pthread_join(tids[i], &retval);
        if (rc != 0) {
            fprintf(stderr, "pthread_join failed at i=%d (rc=%d)\n", i, rc);
            exit_code = 1;
            continue;
        }
        if (retval != NULL) {
            exit_code = 1;
        }
    }

    free(tids);
    free(args);
    return exit_code;
}


