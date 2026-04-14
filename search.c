#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include <getopt.h>
#include "db.h"
#include <sys/stat.h>
#include "search.h"

void show_search_usage(bool fail)
{
    fprintf(stdout,
            "Usage: recmd search [OPTIONS]\n\n"
            "OPTIONS\n"
            " -c, --command Search query to filter commands\n");
    exit(fail ? EXIT_FAILURE : EXIT_SUCCESS);
}

void search_command(int argc, char **argv) {
    opterr = 0;
    const char *search_shortopts = "c:";
    const struct option search_longopts[] = {
        { "command", required_argument, NULL, 'c'},
        { NULL, 0, NULL, 0}
    };

    char *command = NULL;

    int c = 0;
    while ((c = getopt_long(argc, argv, search_shortopts, search_longopts, NULL)) != -1) {
        switch (c) {
            case 'c':
                command = optarg;
                break;
            default:
                show_search_usage(true);
        }
    }

    if (!command)
        show_search_usage(true);

    sqlite3 *db = db_init();
    if (!db) {
        fprintf(stderr, "error initalizating the local db");
        return;
    }

    db_result_t *result = db_fetch_commands(db, command);
    db_close(db);

    print_commands(result);
    db_result_free(result);
}

void print_commands(const db_result_t *result)
{
    if (!result || result->count == 0) {
        printf("no results\n");
        return;
    }
    for (int i = 0; i < result->count; i++)
        printf("%s\n", result->rows[i].command);
}

