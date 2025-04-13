#ifndef ARGS_H
#define ARGS_H

#include "error.h"

struct pipeserver_args {
    char *debug_file;
    char **argv;
    char action;
    char *pipefile;
    char *pidfile;
    char *cmdfile;
    char *command;
};

error read_args(int argc, char *argv[], struct pipeserver_args *args);

#endif
