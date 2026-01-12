#include "workers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>     // fork, getpid
#include <sys/wait.h>   // waitpid
#include <errno.h>

// Print correct usage and exit
static void usage(const char *prog) {
    fprintf(stderr,
            "Usage:\n"
            "  %s <num_processes> cpu\n"
            "  %s <num_processes> mem <mem_mb>\n"
            "  %s <num_processes> io <file_mb> <filepath>\n",
            prog, prog, prog);
    exit(1);
}

// Run the chosen worker in the *current* process
static void run_worker(const char *worker, int worker_index,
                       size_t mem_mb, size_t file_mb, const char *filepath) {
    (void)worker_index; // unused for now (kept for future logging if needed)

    if (strcmp(worker, "cpu") == 0) {
        worker_cpu(BASE_COUNT);
    } else if (strcmp(worker, "mem") == 0) {
        worker_mem(BASE_COUNT, mem_mb);
    } else if (strcmp(worker, "io") == 0) {
        worker_io(BASE_COUNT, filepath, file_mb);
    } else {
        fprintf(stderr, "Unknown worker: %s\n", worker);
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) usage(argv[0]);

    // Parse N (number of processes)
    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "num_processes must be > 0\n");
        return 1;
    }

    const char *worker = argv[2];

    // Defaults (used only for relevant workers)
    size_t mem_mb = 128;
    size_t file_mb = 32;
    const char *filepath = "/tmp/pa01_io.bin";

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
        filepath = argv[4];
        if (file_mb == 0 || filepath == NULL || filepath[0] == '\0') {
            fprintf(stderr, "file_mb must be > 0 and filepath must be valid\n");
            return 1;
        }
    } else {
        fprintf(stderr, "Unknown worker: %s\n", worker);
        usage(argv[0]);
    }

    // Fork N children
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            return 1;
        }

        if (pid == 0) {
            // Child process: run worker and exit
            run_worker(worker, i, mem_mb, file_mb, filepath);
            _exit(0); // use _exit in child after fork
        }

        // Parent continues loop to fork next child
    }

    // Parent: wait for all children
    int status = 0;
    int exit_code = 0;

    for (int i = 0; i < n; i++) {
        pid_t w = waitpid(-1, &status, 0);
        if (w < 0) {
            perror("waitpid failed");
            exit_code = 1;
            break;
        }

        if (WIFEXITED(status)) {
            int child_exit = WEXITSTATUS(status);
            if (child_exit != 0) exit_code = 1;
        } else if (WIFSIGNALED(status)) {
            // Child terminated by signal
            exit_code = 1;
        }
    }

    return exit_code;
}


