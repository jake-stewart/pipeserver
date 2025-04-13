#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>

#include "../include/pipeserver.h"
#include "../include/signal_handler.h"
#include "../include/error.h"
#include "../include/fd_reader.h"
#include "../include/process.h"
#include "../include/args.h"
#include "../include/util.h"
#include "../include/vec.h"

void cleanup(pipeserver_args *args) {
    remove(args->pipefile);
    remove(args->cmdfile);
    remove(args->pidfile);
}

void signal_handler(void *data) {
    pipeserver_args *args = data;
    cleanup(args);
    exit(1);
}

error server_mainloop(
    pipeserver_args *args,
    process *process
) {
    fd_reader reader;
    fd_read_result result;

    while (true) {
        int input_fd = open(
            args->pipefile, O_RDONLY | O_NONBLOCK);
        if (input_fd == -1) {
            return ERROR(
                "failed to open pipe: %s",
                strerror(errno));
        }
        const int n_fds = 3;
        int fds[n_fds] = {
            input_fd,
            process->stdout_fd,
            process->stderr_fd,
        };
        create_fd_reader(&reader, fds, n_fds);

        while (true) {
            ABORT_ERR(fd_reader_read(&reader, &result), {
                close(input_fd);
            })

            if (result.bytes_read == 0) {
                if (result.fd == input_fd) {
                    DEBUG("process stdin reached EOF");
                    close(input_fd);
                    break;
                }
                else {
                    error err = ERROR(
                        "process %s reached EOF",
                        stdfile_name(result.idx));
                    close(input_fd);
                    return err;
                }
            }

            DEBUG("%s: %.*s",
                stdfile_name(result.idx),
                result.bytes_read,
                result.buffer);

            if (result.fd == input_fd) {
                ssize_t bytes_written = write(
                    process->stdin_fd,
                    result.buffer,
                    result.bytes_read);
                if (bytes_written == -1) {
                    error err = ERROR(
                        "failed to write process stdin: %s",
                        strerror(errno));
                    close(input_fd);
                    return err;
                }
            }
        }
    }
}

error start_server(pipeserver_args *args) {
    DEBUG("spawning process");
    process process;
    ABORT_ERR(process_spawn(&process, args->argv), {});

    DEBUG("writing files");
    WITH_FOPEN(args->cmdfile, file, {
        fprintf(file, "%s\n", args->command);
    });
    WITH_FOPEN(args->pidfile, file, {
        fprintf(file, "%d\n", process.pid);
    });

    DEBUG("registering signals");
    int signals[] = {
        SIGINT, SIGHUP, SIGQUIT, SIGTERM,
        SIGTSTP, SIGCONT, SIGWINCH
    };
    ARRAY_FOR_EACH(signals, i, signal, {
        ABORT_ERR(register_signal_handler(
            signal, signal_handler, args), {});
    })

    DEBUG("entering mainloop");
    error err = server_mainloop(args, &process);

    DEBUG("finished mainloop");
    process_close(&process);
    cleanup(args);
    return err;
}

error spawn_server(pipeserver_args *args) {
    DEBUG("creating fifo");
    if (mkfifo(args->pipefile, 0666) == -1) {
        return ERROR(
            "failed to create fifo: %s",
            strerror(errno));
    }
    DEBUG("forking server");
    pid_t pid = fork();
    if (pid == 0) {
        error err = start_server(args);
        exit(err ? err->status : 0);
    }
    else if (pid > 0) {
        return 0;
    }
    else {
        error err = ERROR(
            "failed to fork: %s",
            strerror(errno));
        remove(args->pipefile);
        return err;
    }
}

int is_alive(pipeserver_args *args) {
    return access(args->pidfile, F_OK) == 0;
}

error kill_server(pipeserver_args *args) {
    FILE *f = fopen(args->pidfile, "r");
    if (!f) {
        return ERROR(
            "'%s' is not running",
            args->command);
    }
    int pid;
    if (fscanf(f, "%d", &pid) != 1) {
        return ERROR(
            "'%s' has invalid pid file",
            args->command);
    }
    fclose(f);
    kill(pid, SIGTERM);
    cleanup(args);
    return 0;
}

error list_servers() {
    WITH_OPENDIR(PIPE_DIR, dir, {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (!startswith(entry->d_name, PIPE_PREFIX)
                || !endswith(entry->d_name, ".cmd")
            ) {
                continue;
            }
            char buffer[4096];
            snprintf(buffer, sizeof(buffer), "%s/%s",
                 PIPE_DIR, entry->d_name);
            FILE *file = fopen(buffer, "r");
            if (file) {
                fgets(buffer, sizeof(buffer), file);
                printf("%s", buffer);
                fclose(file);
            }
        }
    });
    return 0;
}

error send_input(pipeserver_args *args) {
    if (!is_alive(args)) {
        ABORT_ERR(spawn_server(args), {});
    }
    int fd = open(args->pipefile, O_WRONLY);
    if (fd == -1) {
        return ERROR(
            "failed to open fifo: %s",
            strerror(errno));
    }
    char buffer[4096];
    ssize_t bytes;
    while (
        (bytes = read(STDIN_FILENO, buffer, 4096)) != 0
    ) {
        if (bytes < 0) {
            close(fd);
            return ERROR(
                "failed to read stdin: %s",
                strerror(errno));
        }
        write(fd, buffer, bytes);
    }
    close(fd);
    return 0;
}

error start(int argc, char *argv[]) {
    pipeserver_args args;
    ABORT_ERR(read_args(argc, argv, &args), {});

    bool provided_command = args.command
        && strlen(args.command) > 0;

    set_debug_file(args.debug_file);

    if (args.action == 'l') {
        return list_servers();
    }
    else if (args.action == 's') {
        if (!provided_command) {
            return STATUS_ERROR(
                2, "missing command for --send");
        }
        return send_input(&args);
    }
    else if (args.action == 'k') {
        if (!provided_command) {
            return STATUS_ERROR(
                2, "missing command for --kill");
        }
        return kill_server(&args);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    error err = start(argc, argv);
    if (err) {
        fprintf(stderr, "%s: %s\n", argv[0], err->message);
        return err->status;
    }
    return 0;
}

