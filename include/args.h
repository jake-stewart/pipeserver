#ifndef ARGS_H
#define ARGS_H

#include "error.h"

typedef struct {
    char *debug_file;
    char **argv;
    char action;
    char *pipefile;
    char *pidfile;
    char *cmdfile;
    char *command;
} pipeserver_args;

error read_args(int argc, char *argv[], pipeserver_args *args);

#endif
