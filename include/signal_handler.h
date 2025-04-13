#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include "../include/error.h"
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

struct signal_handler {
    int sig;
    void (*callback)(void *);
    void *userdata;
};

error register_signal_handler(
    int sig,
    void (*callback)(void *),
    void *userdata
);

#endif
