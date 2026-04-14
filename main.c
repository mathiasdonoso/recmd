#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "recmd.h"
#include <getopt.h>
#include "add.h"
#include "search.h"

void show_usage(bool fail)
{
    /* TODO */
    fprintf(stdout, "Usage: recmd [COMMAND] [OPTIONS]\n\n");
    exit(fail ? EXIT_FAILURE : EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    opterr = 0;
    if (argc < 2) {
        show_usage(false);
        exit(1);
    }

    const char *global_shortopts = "+vVh";
    const struct option global_longopts[] = {
        { "verbose", no_argument, NULL, 'v' },
        { "version", no_argument, NULL, 'V' },
        { "help", no_argument, NULL, 'h' },
        { NULL, 0, NULL, 0 }
    };

    /* int c, flags = 0; */
    int c = 0;
    while ((c = getopt_long(argc, argv, global_shortopts, global_longopts, NULL)) != -1) {
        switch (c) {
            case 'v':
                /* TODO */
                /* flags |= RECMD_FLAG_VERBOSE; */
                break;
            case 'V':
                printf("%s\n", RECMD_RELVER);
                exit(EXIT_SUCCESS);
            case 'h':
                show_usage(false);
            default:
                break;
        }
    }

    char *subcommand = argv[optind];
    if (!subcommand) {
        show_usage(true);
    }

    if (strcmp(subcommand, "add") == 0) {
        add_command(argc - optind, argv + optind);
    } else if (strcmp(subcommand, "search") == 0) {
        search_command(argc - optind, argv + optind);
    } else {
        show_usage(true);
    }

    exit(0);
}
