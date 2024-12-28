#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <errno.h>

// Define the memory range to be monitored
#define WATCHED_MEMORY_START 0x600000  // Example start address for monitored memory
#define WATCHED_MEMORY_END   0x610000  // Example end address for monitored memory

// Check if an address is within the monitored memory range
int is_watched_memory(uint64_t addr) {
    return (addr >= WATCHED_MEMORY_START && addr < WATCHED_MEMORY_END);
}

// Monitor a process for potential pointer corruption
void monitor_process(pid_t pid) {
    int status;
    struct user_regs_struct regs;

    while (1) {
        // Wait for the child process to stop or terminate
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status)) {
            printf("Target process exited with status %d.\n", WEXITSTATUS(status));
            break;
        }

        if (WIFSIGNALED(status)) {
            printf("Target process killed by signal %d.\n", WTERMSIG(status));
            break;
        }

        if (WIFSTOPPED(status)) {
            int sig = WSTOPSIG(status);
            printf("Target process stopped by signal %d.\n", sig);
        }

        // Retrieve the current register state of the process
        if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1) {
            perror("PTRACE_GETREGS failed");
            exit(EXIT_FAILURE);
        }

        // Check if the instruction pointer (RIP) is accessing monitored memory
        if (is_watched_memory(regs.rip)) {
            printf("Pointer access violation detected at RIP: 0x%llx\n", regs.rip);
            printf("Investigate potential pointer corruption!\n");
        }

        // Resume the child process
        if (ptrace(PTRACE_CONT, pid, NULL, NULL) == -1) {
            perror("PTRACE_CONT failed");
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program_to_monitor>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Fork to create a child process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // Child process: prepare for tracing and execute the target program
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
            perror("PTRACE_TRACEME failed");
            exit(EXIT_FAILURE);
        }

        execvp(argv[1], &argv[1]);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process: monitor the child process
        monitor_process(pid);
    }

    return EXIT_SUCCESS;
}
