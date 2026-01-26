# PA01: Processes and Threads

**Roll Number:** MT25045

**Course:** Graduate Systems (CSE638)

---

## Overview

This assignment implements and analyzes performance differences between process-based (fork) and thread-based (pthread) execution models using three workload types: CPU-intensive, Memory-intensive, and I/O-intensive.

## File Structure

```
MT25045_PA01/
├── MT25045_Part_A_Program_A.c    # Process-based program using fork()
├── MT25045_Part_A_Program_B.c    # Thread-based program using pthread
├── MT25045_Part_B_workers.c      # Worker function implementations
├── MT25045_Part_B_workers.h      # Worker function header (LOOP_COUNT=5000)
├── MT25045_Part_C_script.sh      # Automated measurement script
├── MT25045_Part_C_CSV.csv        # Part C results (generated)
├── MT25045_Part_D_script.sh      # Scaling analysis script
├── MT25045_Part_D_CSV.csv        # Part D results (generated)
├── MT25045_Part_D_plots.png      # Visualization plots (generated)
├── MT25045_Part_D_plots.pdf      # Plots in PDF (generated)
├── MT25045_Part_D_memory.png     # Memory scaling plot (generated)
├── MT25045_Report.pdf            # Analysis report
├── Makefile                      # Build automation
└── README.md                     # This file
```

## Compilation

```bash
make          # Build all programs
make clean    # Remove compiled files
```

## Usage

### Program A (Processes)
```bash
./program_a <cpu|mem|io> [num_processes]

# Examples:
./program_a cpu 2    # 2 child processes, CPU-intensive
./program_a mem 4    # 4 child processes, memory-intensive
./program_a io 3     # 3 child processes, I/O-intensive
```

### Program B (Threads)
```bash
./program_b <cpu|mem|io> [num_threads]

# Examples:
./program_b cpu 2    # 2 threads, CPU-intensive
./program_b mem 8    # 8 threads, memory-intensive
```

### Running with CPU Pinning
```bash
taskset -c 0,1 ./program_a cpu 2
```

### Running Automated Tests

```bash
# Part C
chmod +x MT25045_Part_C_script.sh
./MT25045_Part_C_script.sh

# Part D
chmod +x MT25045_Part_D_script.sh
./MT25045_Part_D_script.sh
```

## Worker Functions

| Worker | Description | LOOP_COUNT |
|--------|-------------|------------|
| cpu | Leibniz π calculation + trigonometric ops | 5000 |
| mem | 8MB array with random access patterns | 5000 |
| io | File read/write (256KB per iteration) | 5000 |

## Dependencies

- GCC compiler
- pthread library
- Math library (-lm)
- Python 3 with pandas, matplotlib
- sysstat (for iostat)

## Author

- **Roll Number:** MT25045
- **Date:** January 2026

