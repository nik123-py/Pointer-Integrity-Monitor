# Pointer-Integrity-Monitor
## Overview
This program monitors a child process for potential pointer corruption by observing its memory accesses using the `ptrace` system call. It checks whether the instruction pointer (RIP) accesses a defined memory range and reports any violations.

## Features
- Uses `ptrace` to trace system calls and memory access.
- Monitors a specific memory range for suspicious accesses.
- Reports violations in the monitored range.
- Handles process termination and signals gracefully.

## Requirements
- GCC (or any C compiler)
- Linux-based operating system (supports `ptrace` API)

## Compilation
To compile the program, use the following command:
```bash
gcc -o monitor monitor.c
```

## Usage
Run the program with the target executable as an argument:
```bash
./monitor <program_to_monitor>
```
For example:
```bash
./monitor ./test_program
```

## How It Works
1. The program forks to create a child process.
2. The child process sets itself up for tracing using `PTRACE_TRACEME` and executes the target program.
3. The parent process monitors the child using `PTRACE_GETREGS` to read the child's register state.
4. If the instruction pointer (RIP) accesses a defined memory range, a warning is printed.
5. The parent continues the child process until it exits or is terminated.

## Code Highlights
- **Monitored Memory Range:**
  The memory range to be monitored is defined with:
  ```c
  #define WATCHED_MEMORY_START 0x600000
  #define WATCHED_MEMORY_END   0x610000
  ```
  Adjust these values as needed.

- **Pointer Violation Check:**
  The function `is_watched_memory` determines if an address falls within the monitored range.

- **Process Monitoring:**
  The `monitor_process` function uses `ptrace` to inspect and manage the traced process.

## Example Output
When a pointer access violation occurs, the program will output:
```
Pointer access violation detected at RIP: 0x<address>
Investigate potential pointer corruption!
```

## Limitations
- This program is designed for educational purposes and debugging.
- It assumes the memory range is meaningful for the target application.
- Does not handle multi-threaded child processes.

## Troubleshooting
- **Compilation Errors:** Ensure all required headers are available and the system supports `ptrace`.
- **Permission Errors:** Run the program as a user with sufficient permissions to trace processes.
- **Unexpected Behavior:** Verify the target program and adjust the monitored memory range as needed.

## License
This program is open-source and available under the MIT License. Modify and distribute as needed.
