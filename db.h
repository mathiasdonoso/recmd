#ifndef DB_H
#define DB_H

#include <sqlite3.h>

typedef struct {
    char *command;
    char *directory;
    int   exit_code;
    char *ts;
} db_row_t;

typedef struct {
    db_row_t *rows;
    int       count;
} db_result_t;

void db_result_free(db_result_t *result);

sqlite3     *db_init(void);
void         db_close(sqlite3 *db);

int          db_record_command(sqlite3 *db, const char *cmd, const char *dir, int exit_code);
db_result_t *db_fetch_all(sqlite3 *db);

#endif
