#ifndef ERROR_H
#define ERROR_H

#include <unistd.h>

struct error_details {
    const char *message;
    int status;
};

typedef struct error_details* error;

void set_debug_file(const char *debug_file);

void debug_log(const char *prefix, int line, const char *format, ...);

error error_log(int status, int line, const char *format, ...);

#define DEBUG(...) debug_log("DEBUG", __LINE__, __VA_ARGS__)
#define ERROR(...) error_log(1, __LINE__, __VA_ARGS__)
#define STATUS_ERROR(status, ...) error_log(status, __LINE__, __VA_ARGS__)
#define ABORT_ERR(code, cleanup) { error err = code; if (err) { cleanup; return err; } }

#endif
