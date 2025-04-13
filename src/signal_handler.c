#include "../include/signal_handler.h"
#include "../include/error.h"
#include "../include/vec.h"

vec handlers = vec_init(struct signal_handler);

void main_handler(int sig) {
    for (int i = 0; i < handlers.len; i++) {
        struct signal_handler *handler;
        vec_get(handler, &handlers, i);
        if (handler->sig == sig) {
            handler->callback(handler->userdata);
        }
    }
}

error register_signal_handler(
    int sig,
    void (*callback)(void *),
    void *userdata
) {
    bool signal_registered = false;
    for (int i = 0; i < handlers.len; i++) {
        struct signal_handler *handler;
        vec_get(handler, &handlers, i);
        if (handler->sig == sig) {
            signal_registered = true;
            break;
        }
    }
    if (!signal_registered) {
        signal(sig, main_handler);
    }

    struct signal_handler handler = {
        .sig = sig,
        .callback = callback,
        .userdata = userdata,
    };
    vec_push(&handlers, handler);

    return 0;
}
