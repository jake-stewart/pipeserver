#include "../include/error.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>

const char *debug_file = NULL;
char errormsg[1024] = "something went wrong";
struct error_details error_details;

void set_debug_file(const char *file) {
    debug_file = file;
}

void debug_log(
    const char *prefix,
    int line,
    const char *format,
    ...
) {
    if (debug_file == NULL) {
        return;
    }
    int fd = open(
        debug_file,
        O_RDWR | O_APPEND | O_CREAT,
        0644
    );
    struct flock lock = {
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0,
    };
    fcntl(fd, F_SETLKW, &lock);
    va_list args;
    va_start(args, format);
    dprintf(fd, "[%s] L%d: ", prefix, line);
    vdprintf(fd, format, args);
    dprintf(fd, "\n");
    va_end(args);
    fsync(fd);
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
}

error error_log(
    int status,
    int line,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    vsnprintf(errormsg, sizeof errormsg, format, args);
    debug_log("ERROR", line, format, args);
    va_end(args);
    error_details.message = errormsg;
    error_details.status = status;
    return &error_details;
}
