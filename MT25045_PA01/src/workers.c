#include "workers.h"

#include <stdio.h>     // printf, perror
#include <stdlib.h>    // malloc, free, exit
#include <stdint.h>    // uint64_t
#include <unistd.h>    // lseek, fsync, close, unlink
#include <fcntl.h>     // open flags
#include <sys/types.h>
#include <sys/stat.h>  // file permissions

// -------------------- CPU WORKER --------------------
// CPU-heavy loop: does arithmetic repeatedly.
// "volatile" prevents the compiler from optimizing the loop away.
void worker_cpu(unsigned long count) {
    volatile double acc = 1.0;

    for (unsigned long i = 1; i <= count; i++) {
        acc += (double)i * 1.000001;
        acc *= 0.999999;
        if (acc > 1e12) acc = 1.0;
    }

    // Print result so computation is "observable"
    printf("[cpu] done, acc=%.3f\n", (double)acc);
}

// -------------------- MEM WORKER --------------------
// Memory-heavy: allocate a big array and repeatedly touch it with a stride.
// This creates lots of RAM/cache traffic.
void worker_mem(unsigned long count, size_t mem_mb) {
    size_t bytes = mem_mb * 1024UL * 1024UL;
    size_t n = bytes / sizeof(uint64_t);
    if (n < 1024) n = 1024;  // minimum size safety

    uint64_t *arr = (uint64_t *)malloc(n * sizeof(uint64_t));
    if (!arr) {
        perror("[mem] malloc failed");
        exit(1);
    }

    // Touch memory once to make sure pages are actually allocated
    for (size_t i = 0; i < n; i++) {
        arr[i] = (uint64_t)i;
    }

    // Stride access reduces cache friendliness
    size_t stride = 64; // 64 * 8 bytes = 512-byte jumps
    volatile uint64_t sum = 0;

    for (unsigned long it = 0; it < count; it++) {
        for (size_t i = 0; i < n; i += stride) {
            arr[i] ^= (uint64_t)it; // write
            sum += arr[i];          // read
        }
    }

    printf("[mem] done, sum=%llu (mem_mb=%zu)\n",
           (unsigned long long)sum, mem_mb);

    free(arr);
}

// -------------------- IO WORKER --------------------
// I/O-heavy: write and read a file repeatedly.
// fsync() forces data to disk, making it truly I/O-bound.
void worker_io(unsigned long count, const char *filepath, size_t file_mb) {
    size_t bytes = file_mb * 1024UL * 1024UL;
    if (bytes < 1024 * 1024) bytes = 1024 * 1024; // minimum 1MB

    char *buf = (char *)malloc(bytes);
    if (!buf) {
        perror("[io] malloc failed");
        exit(1);
    }

    // Fill buffer with deterministic data
    for (size_t i = 0; i < bytes; i++) {
        buf[i] = (char)('A' + (i % 26));
    }

    int fd = open(filepath, O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (fd < 0) {
        perror("[io] open failed");
        free(buf);
        exit(1);
    }

    volatile unsigned long long checksum = 0;

    for (unsigned long it = 0; it < count; it++) {
        // Write whole buffer
        ssize_t written = write(fd, buf, bytes);
        if (written < 0) {
            perror("[io] write failed");
            close(fd);
            free(buf);
            exit(1);
        }

        // Force flush to disk
        if (fsync(fd) != 0) {
            perror("[io] fsync failed");
            close(fd);
            free(buf);
            exit(1);
        }

        // Rewind and read back
        if (lseek(fd, 0, SEEK_SET) < 0) {
            perror("[io] lseek failed");
            close(fd);
            free(buf);
            exit(1);
        }

        ssize_t r = read(fd, buf, bytes);
        if (r < 0) {
            perror("[io] read failed");
            close(fd);
            free(buf);
            exit(1);
        }

        // Use some read data so compiler can’t treat it as unused
        checksum += (unsigned long long)buf[it % bytes];
    }

    printf("[io] done, checksum=%llu (file_mb=%zu) file=%s\n",
           checksum, file_mb, filepath);

    close(fd);
    free(buf);

    // Cleanup so repeated runs start clean (optional)
    unlink(filepath);
}