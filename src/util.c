#include "../include/util.h"
#include <unistd.h>
#include <string.h>

const char* stdfile_name(int fd) {
    switch (fd) {
        case STDIN_FILENO:
            return "stdin";
        case STDOUT_FILENO:
            return "stdout";
        case STDERR_FILENO:
            return "stderr";
        default:
            return "unknown";
    }
}

bool startswith(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

bool endswith(const char *str, const char *postfix) {
    size_t str_len = strlen(str);
    size_t postfix_len = strlen(postfix);
    if (postfix_len > str_len) {
        return false;
    }
    return strcmp(str + str_len - postfix_len, postfix) == 0;
}

