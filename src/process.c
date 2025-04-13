#include "../include/process.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>

error process_spawn(struct process *process, char **args) {
    int pipes[3][2];

    for (int i = 0; i < 3; i++) {
        if (pipe(pipes[i]) == -1) {
            error err = ERROR("failed to create pipe: %s",
                  strerror(errno));
            for (int j = i - 1; j <= 0; j--) {
                close(pipes[j][PIPE_READ]);
                close(pipes[j][PIPE_WRITE]);
            }
            return err;
        }
    }

    pid_t pid = fork();
    if (pid == 0) {
        close(pipes[STDIN_FILENO][PIPE_WRITE]);
        close(pipes[STDOUT_FILENO][PIPE_READ]);
        close(pipes[STDERR_FILENO][PIPE_READ]);
        dup2(pipes[STDIN_FILENO][PIPE_READ], STDIN_FILENO);
        dup2(pipes[STDOUT_FILENO][PIPE_WRITE], STDOUT_FILENO);
        dup2(pipes[STDERR_FILENO][PIPE_WRITE], STDERR_FILENO);
        execvp(args[0], args);
        ERROR("failed to start process: %s", strerror(errno));
        close(pipes[STDIN_FILENO][PIPE_READ]);
        close(pipes[STDOUT_FILENO][PIPE_WRITE]);
        close(pipes[STDERR_FILENO][PIPE_WRITE]);
        exit(1);
    }
    else if (pid > 0) {
        close(pipes[STDIN_FILENO][PIPE_READ]);
        close(pipes[STDOUT_FILENO][PIPE_WRITE]);
        close(pipes[STDERR_FILENO][PIPE_WRITE]);
        process->pid = pid;
        process->stdin_fd = pipes[STDIN_FILENO][PIPE_WRITE];
        process->stdout_fd = pipes[STDOUT_FILENO][PIPE_READ];
        process->stderr_fd = pipes[STDERR_FILENO][PIPE_READ];
        return 0;
    }
    else {
        error err = ERROR("failed to spawn process: %s",
             strerror(errno));
        for (int i = 0; i < 3; i++) {
            close(pipes[i][PIPE_READ]);
            close(pipes[i][PIPE_WRITE]);
        }
        return err;
    }
}

void process_close(struct process *process) {
    close(process->stdin_fd);
    close(process->stdout_fd);
    close(process->stderr_fd);
}
