#ifndef FD_READER_H
#define FD_READER_H

#include <sys/select.h>
#include <unistd.h>
#include "error.h"
#include "process.h"

typedef struct {
    process *process;
    unsigned int fd_index;
    fd_set fd_set;
    int *fds;
    int n_fds;
} fd_reader;

typedef struct {
    int idx;
    int fd;
    char buffer[4096];
    ssize_t bytes_read;
} fd_read_result;

void create_fd_reader(
    fd_reader *fd_reader,
    int *fds,
    int n_fds
);

error fd_reader_read(
    fd_reader *reader,
    fd_read_result *result
);

#endif
