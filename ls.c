#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include <getopt.h>
#include "db.h"
#include <sys/stat.h>
#include "ls.h"

void show_ls_usage(bool fail)
{
    fprintf(stdout,
            "Usage: recmd ls\n\n");
    exit(fail ? EXIT_FAILURE : EXIT_SUCCESS);
}

void ls_command(int argc, char **argv) {
    opterr = 0;
    const char *ls_shortopts = "";
    const struct option ls_longopts[] = {
        { NULL, 0, NULL, 0}
    };

    int c = 0;
    while ((c = getopt_long(argc, argv, ls_shortopts, ls_longopts, NULL)) != -1) {
        switch (c) {
            default:
                show_ls_usage(true);
        }
    }

    sqlite3 *db = db_init();
    if (!db) {
        fprintf(stderr, "error initalizating the local db");
        return;
    }

    db_result_t *result = db_fetch_all(db);
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
