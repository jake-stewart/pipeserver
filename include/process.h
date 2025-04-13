#ifndef PROCESS_H
#define PROCESS_H

#include "error.h"

#define PIPE_READ 0
#define PIPE_WRITE 1

typedef struct {
    pid_t pid;
    int stdin_fd;
    int stdout_fd;
    int stderr_fd;
} process;

error process_spawn(process *process, char **args);

void process_close(process *process);

#endif
