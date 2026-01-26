/*
 * MT25045_Part_B_workers.c - Implementation of CPU, Memory, and I/O intensive workers
 * Roll No: MT25045
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include "MT25045_Part_B_workers.h"

/*
 * CPU-intensive worker: Calculates digits of pi using Leibniz formula
 * and performs heavy mathematical computations
 */
void worker_cpu(void) {
    volatile double pi = 0.0;
    volatile double result = 0.0;
    
    for (int iter = 0; iter < LOOP_COUNT; iter++) {
        /* Leibniz formula for pi: pi/4 = 1 - 1/3 + 1/5 - 1/7 + ... */
        pi = 0.0;
        for (int i = 0; i < 100000; i++) {
            pi += (i % 2 == 0 ? 1.0 : -1.0) / (2.0 * i + 1.0);
        }
        pi *= 4.0;
        
        /* Additional CPU-intensive calculations */
        for (int i = 1; i <= 1000; i++) {
            result += sin(i) * cos(i) * tan(i * 0.001);
            result += sqrt(i) * log(i + 1);
            result += pow(i, 1.5) / (i + 1);
        }
    }
    
    /* Prevent compiler optimization */
    if (result == 0.123456789) printf("Result: %f, Pi: %f\n", result, pi);
}

/*
 * Memory-intensive worker: Allocates large arrays and performs
 * random access patterns to stress the memory subsystem
 * 
 * NOTE: Using 8MB for VM with limited RAM (4GB)
 */
void worker_mem(void) {
    /* Allocate array (8 MB - safe for 4GB RAM VM) */
    size_t array_size = 8 * 1024 * 1024 / sizeof(long);
    long *large_array = (long *)malloc(array_size * sizeof(long));
    
    if (!large_array) {
        perror("Failed to allocate memory");
        return;
    }
    
    for (int iter = 0; iter < LOOP_COUNT; iter++) {
        /* Sequential write - fills cache lines */
        for (size_t i = 0; i < array_size; i++) {
            large_array[i] = i * iter;
        }
        
        /* Random access pattern - causes cache misses */
        volatile long sum = 0;
        size_t access_count = array_size / 2;
        for (size_t i = 0; i < access_count; i++) {
            /* Pseudo-random index using simple hash */
            size_t idx = (i * 2654435761UL) % array_size;
            sum += large_array[idx];
            large_array[idx] = sum % 1000000;
        }
        
        /* Memory copy operations */
        size_t half = array_size / 2;
        memmove(&large_array[half], large_array, half * sizeof(long));
    }
    
    free(large_array);
}

/*
 * I/O-intensive worker: Performs heavy file read/write operations
 * 
 * NOTE: Reduced writes for VM with limited disk (10GB)
 */
void worker_io(void) {
    /* Use PID in filename to avoid conflicts between processes */
    char filename[64];
    snprintf(filename, sizeof(filename), "/tmp/io_worker_%d.dat", getpid());
    
    const size_t buf_size = 4096;  /* 4KB buffer */
    char *buffer = (char *)malloc(buf_size);
    
    if (!buffer) {
        perror("Failed to allocate buffer");
        return;
    }
    
    /* Fill buffer with data */
    memset(buffer, 'A', buf_size);
    
    for (int iter = 0; iter < LOOP_COUNT; iter++) {
        /* Write phase */
        int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("Failed to open file for writing");
            free(buffer);
            return;
        }
        
        /* Write 64 blocks (256KB per iteration) */
        for (int i = 0; i < 64; i++) {
            buffer[0] = (char)(iter % 256);
            buffer[buf_size - 1] = (char)(i % 256);
            write(fd, buffer, buf_size);
        }
        
        /* Force sync to disk */
        fsync(fd);
        close(fd);
        
        /* Read phase */
        fd = open(filename, O_RDONLY);
        if (fd < 0) {
            perror("Failed to open file for reading");
            free(buffer);
            return;
        }
        
        /* Read all blocks */
        while (read(fd, buffer, buf_size) > 0) {
            volatile char c = buffer[0];
            (void)c;
        }
        
        close(fd);
    }
    
    /* Cleanup */
    unlink(filename);
    free(buffer);
}

