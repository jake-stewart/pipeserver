#include "../include/fd_reader.h"
#include "../include/util.h"
#include <errno.h>
#include <string.h>

void create_fd_reader(
    struct fd_reader *fd_reader,
    int *fds,
    int n_fds
) {
    fd_reader->fds = fds;
    fd_reader->n_fds = n_fds;
    fd_reader->fd_index = -1;
}

error _fd_reader_reset(struct fd_reader *reader) {
    reader->fd_index = 0;
    FD_ZERO(&reader->fd_set);
    int max_fd = -1;
    for (int i = 0; i < reader->n_fds; i++) {
        if (reader->fds[i] >= 0) {
            if (reader->fds[i] > max_fd) {
                max_fd = reader->fds[i];
            }
            FD_SET(reader->fds[i], &reader->fd_set);
        }
    }

    if (max_fd == -1) {
        return ERROR("no fds to read from");
    }

    int ret = select(
        max_fd + 1, &reader->fd_set, NULL, NULL, NULL);

    if (ret == -1) {
        return ERROR("failed to poll fds: %s", strerror(errno));
    }
    return 0;
}

error _fd_reader_next(
    struct fd_reader *reader,
    struct fd_read_result *result,
    bool *found
) {
    int idx = reader->fd_index++;
    int fd = reader->fds[idx];
    if (fd == -1) {
        *found = false;
        return 0;
    }
    if (FD_ISSET(fd, &reader->fd_set)) {
        result->fd = fd;
        result->bytes_read = read(
            fd, result->buffer, sizeof(result->buffer));
        if (result->bytes_read <= 0) {
            reader->fds[idx] = -1;
            if (result->bytes_read < 0) {
                if (fd <= 2) {
                    return ERROR("failed to read %s: %s",
                          stdfile_name(idx), strerror(errno));
                }
                else {
                    return ERROR(
                        "failed to read from file descriptor "
                        "%d: %s", fd, strerror(errno));
                }
            }
        }
        *found = true;
        return 0;
    }
    *found = false;
    return 0;
}

error fd_reader_read(
    struct fd_reader *reader,
    struct fd_read_result *result
) {
    while (true) {
        while (reader->fd_index < reader->n_fds) {
            bool found;
            ABORT_ERR(_fd_reader_next(reader, result, &found), {});
            if (found) {
                return 0;
            }
        }
        ABORT_ERR(_fd_reader_reset(reader), {});
    }
}
