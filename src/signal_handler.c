#include "../include/signal_handler.h"
#include "../include/error.h"
#include "../include/vec.h"
#include <stdbool.h>

typedef struct {
    int sig;
    void (*callback)(void *);
    void *userdata;
} signal_handler;

DEFINE_VEC(signal_handler);

signal_handler_vec handlers = VEC_ZERO();

void main_handler(int sig) {
    VEC_FOR_EACH(handlers, i, handler, {
        if (handler.sig == sig) {
            handler.callback(handler.userdata);
        }
    })
}

error register_signal_handler(
    int sig,
    void (*callback)(void *),
    void *userdata
) {
    bool signal_registered = false;
    VEC_FOR_EACH(handlers, i, handler, {
        if (handler.sig == sig) {
            signal_registered = true;
            break;
        }
    });
    if (!signal_registered) {
        signal(sig, main_handler);
    }

    signal_handler handler = {
        .sig = sig,
        .callback = callback,
        .userdata = userdata,
    };
    signal_handler_vec_push(handlers, handler);

    return 0;
}
