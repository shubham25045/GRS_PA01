#ifndef WORKERS_H
#define WORKERS_H

#include <stddef.h>   // size_t

// My roll last digit = 5, so count = 5 * 10^3 = 5000
#define ROLL_LAST_DIGIT 5
#define BASE_COUNT (ROLL_LAST_DIGIT * 1000UL)

// Worker functions
void worker_cpu(unsigned long count);
void worker_mem(unsigned long count, size_t mem_mb);
void worker_io(unsigned long count, const char *filepath, size_t file_mb);

#endif
