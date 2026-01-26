/*
 * MT25045_Part_B_workers.h - Header file for worker functions
 * Roll No: MT25045
 */

#ifndef WORKERS_H
#define WORKERS_H

/* 
 * LOOP_COUNT: Last digit of roll number × 10^3
 * Roll number: MT25045, last digit: 5
 * LOOP_COUNT = 5 × 1000 = 5000
 */
#define LOOP_COUNT 5000

/* Worker function declarations */
void worker_cpu(void);
void worker_mem(void);
void worker_io(void);

#endif /* WORKERS_H */

