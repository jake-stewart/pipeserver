#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include "../include/error.h"

error register_signal_handler(
    int sig,
    void (*callback)(void *),
    void *userdata
);

#endif
