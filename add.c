#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include <getopt.h>
#include "db.h"
#include <sys/stat.h>

void show_add_usage(bool fail)
{
    fprintf(stdout,
            "Usage: recmd add [OPTIONS]\n\n"
            "OPTIONS\n"
            " -c, --command    Command to store\n"
            " -d, --directory  Working directory where the command was run\n"
            " -e, --exitcode   Exit code of the command\n");
    exit(fail ? EXIT_FAILURE : EXIT_SUCCESS);
}

void add_command(int argc, char **argv) {
    opterr = 0;
    const char *add_shortopts = "c:d:e:";
    const struct option add_longopts[] = {
        { "command", required_argument, NULL, 'c'},
        { "directory", required_argument, NULL, 'd' },
        { "exitcode", required_argument, NULL, 'e' },
        { NULL, 0, NULL, 0}
    };

    int exitcode = 0;
    char *directory = NULL, *command = NULL;

    int c = 0;
    while ((c = getopt_long(argc, argv, add_shortopts, add_longopts, NULL)) != -1) {
        switch (c) {
            case 'c':
                command = optarg;
                break;
            case 'd':
                directory = optarg;
                break;
            case 'e':
                exitcode = atoi(optarg);
                break;
            default:
                show_add_usage(true);
        }
    }

    if (!directory || !command)
        show_add_usage(true);

    sqlite3 *db = db_init();
    if (!db) {
        fprintf(stderr, "error initalizating the local db");
        return;
    }

    int rc = db_record_command(db, command, directory, exitcode);
    db_close(db);

    if (rc != 0) {
        fprintf(stderr, "error recording command: %s\n", command);
        return;
    }
}

