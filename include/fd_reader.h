#ifndef FD_READER_H
#define FD_READER_H

#include <sys/select.h>
#include <unistd.h>
#include "error.h"

struct fd_reader {
    struct process *process;
    unsigned int fd_index;
    fd_set fd_set;
    int *fds;
    int n_fds;
};

struct fd_read_result {
    int idx;
    int fd;
    char buffer[4096];
    ssize_t bytes_read;
};

void create_fd_reader(
    struct fd_reader *fd_reader,
    int *fds,
    int n_fds
);

error fd_reader_read(
    struct fd_reader *reader,
    struct fd_read_result *result
);

#endif
