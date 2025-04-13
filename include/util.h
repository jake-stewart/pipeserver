#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

const char* stdfile_name(int fd);

#define WITH_FOPEN(fname, file, code) { \
    FILE *file = fopen(fname, "w"); \
    if (file == NULL) { \
        return ERROR("failed to open '%s': %s", fname, strerror(errno)); \
    } \
    code \
    fclose(file); \
}

#define WITH_OPENDIR(dirname, dir, code) { \
    DIR *dir = opendir(PIPE_DIR); \
    if (!dir) { \
        return ERROR("failed to open '%s': %s", dirname, strerror(errno)); \
    } \
    code \
    closedir(dir); \
}

bool startswith(const char *str, const char *prefix);
bool endswith(const char *str, const char *postfix);

#endif
