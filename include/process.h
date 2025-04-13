#ifndef PROCESS_H
#define PROCESS_H

#include "error.h"

#define PIPE_READ 0
#define PIPE_WRITE 1

struct process {
    pid_t pid;
    int stdin_fd;
    int stdout_fd;
    int stderr_fd;
};

error process_spawn(struct process *process, char **args);

void process_close(struct process *process);

#endif
