# Matrix-Multiplication (Multi-Threading)

## Overview

This project implements matrix multiplication in C using three different approaches:

1. **Sequential Multiplication** (Standard method)
2. **Thread-per-Row Multiplication** (Each row is computed in a separate thread)
3. **Thread-per-Element Multiplication** (Each element is computed in a separate thread)

The program reads two matrices from text files (`a.txt` and `b.txt`), multiplies them using different methods, and writes the resulting matrix to corresponding output files.

---

## Features

- **Efficient Memory Allocation:** Uses dynamic memory allocation for flexible matrix sizes.
- **Multithreading with Pthreads:** Implements parallel processing to speed up matrix multiplication.
- **Performance Measurement:** Uses `gettimeofday` to measure execution time in microseconds.
- **File I/O Operations:** Reads matrix data from files and writes the results to output files.

---

## Prerequisites

- GCC Compiler (`gcc`)
- POSIX Threads Library (`pthread`)
- Linux/Unix environment (WSL recommended for Windows users)

---

## Compilation & Execution

### **Compile the program:**

```sh
gcc MatrixMultiplication.c -o MatrixMultiplication
```

### **Run the program:**

```sh
./MatrixMultiplication
```

---

## Input File Format

Each matrix file (`a.txt`, `b.txt`) should follow this format:

```
row=<number_of_rows> col=<number_of_columns>
value11 value12 ... value1N
value21 value22 ... value2N
...
```

Example (`a.txt`):

```
row=2 col=3
1 2 3
4 5 6
```

Example (`b.txt`):

```
row=3 col=2
7 8
9 10
11 12
```

---

## Output Files

- `c_per_matrix.txt` (Result using normal multiplication)
- `c_per_row.txt` (Result using thread-per-row multiplication)
- `c_per_element.txt` (Result using thread-per-element multiplication)

Each file follows the same format as input files.

---

## Performance Analysis

The program measures execution time for each method and prints the results in seconds and microseconds:

```
Normal Method takes X seconds Y microseconds
Thread-per-row takes X seconds Y microseconds
Thread-per-element takes X seconds Y microseconds
```

---

## Potential Enhancements

- Implement thread pooling for improved performance.
- Optimize memory management to reduce allocation overhead.
- Add support for larger matrices with more efficient storage.

---

## Author

Mina Nabil

