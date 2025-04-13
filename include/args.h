#ifndef ARGS_H
#define ARGS_H

#include "error.h"

struct pipeserver_args {
    char *debug_file;
    char path[4096];
    char command[4096];
    char **argv;
    char action;
    char *pipefile;
    char *pidfile;
    char *cmdfile;
};

error read_args(int argc, char *argv[], struct pipeserver_args *args);

#endif
