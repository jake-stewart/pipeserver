#include "../include/args.h"
#include "../include/pipeserver.h"
#include <getopt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage() {
    printf("USAGE: pipeserver {OPTIONS}\n\n");
    printf("OPTIONS:\n");
    printf("    --help                 show this message\n");
    printf("    --list                 list active pipeservers\n");
    printf("    --debug <FILE>         log debug messages to FILE\n");
    printf("    --kill <CMD> <ARGS>    stop the pipeserver running CMD with ARGS\n");
    printf("    --send <CMD> <ARGS>    feed STDIN into the pipeserver running CMD with ARGS\n");
    printf("                           if a pipeserver does not exist, one will be created\n");
}

error read_args(int argc, char *argv[], struct pipeserver_args *args) {
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"list", no_argument, 0, 'l'},
        {"kill", no_argument, 0, 'k'},
        {"send", no_argument, 0, 's'},
        {"debug", required_argument, 0, 'd'},
        {0, 0, 0, 0}
    };

    args->debug_file = NULL;
    args->argv = NULL;
    args->path[0] = 0;
    args->command[0] = 0;
    args->action = 0;
    args->pipefile = NULL;
    args->pidfile = NULL;
    args->cmdfile = NULL;

    int opt;
    int idx = 0;

    while ((opt = getopt_long(argc, argv, "h l f k s d:", long_options, &idx)) != -1) {
        switch (opt) {
            case 'h':
                usage();
                exit(0);
            case 'l':
            case 'k':
            case 's':
                if (args->action) {
                    return ERROR("multiple commands specified");
                }
                args->action = opt;
                break;
            case 'd':
                args->debug_file = optarg;
                break;
            case '?':
                exit(2);
                break;
            default:
                return ERROR("unexpected option");
        }
    }

    if (!args->action) {
        usage();
        exit(2);
    }

    if (argc > optind) {
        strcpy(args->path, PIPE_DIR "/" PIPE_PREFIX);
        for (int i = optind; i < argc; i++) {
            if (strlen(args->path) + strlen(argv[i]) > 4000) {
                return ERROR("command is too long");
            }
            if (strlen(args->command)) {
                strcat(args->path, "_");
                strcat(args->command, " ");
            }
            strcat(args->path, argv[i]);
            strcat(args->command, argv[i]);
        }
        args->argv = &argv[optind];
        asprintf(&args->pipefile, "%s%s", args->path, ".pipe");
        asprintf(&args->pidfile, "%s%s", args->path, ".pid");
        asprintf(&args->cmdfile, "%s%s", args->path, ".cmd");
    }

    return 0;
}
