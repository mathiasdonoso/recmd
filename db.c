#include "sqlite3.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "db.h"

static const char *db_path(void)
{
    static char path[256];

    const char *override = getenv("RECMD_DB_PATH");
    if (override) {
        if (strlen(override) >= sizeof(path)) {
            fprintf(stderr, "RECMD_DB_PATH too long\n");
            return NULL;
        }
        strncpy(path, override, sizeof(path) - 1);
        path[sizeof(path) - 1] = '\0';
        return path;
    }

    const char *home = getenv("HOME");
    if (!home) {
        fprintf(stderr, "$HOME not set\n");
        return NULL;
    }

    int n = snprintf(path, sizeof(path), "%s/.config/recmd", home);
    if (n < 0 || n >= (int)sizeof(path)) {
        fprintf(stderr, "path too long\n");
        return NULL;
    }

    int dir_result = mkdir(path, 0755);
    if(dir_result != 0 && errno != EEXIST) {
        fprintf(stderr, "cannot create the db directory");
        return NULL;
    }

    n = snprintf(path, sizeof(path), "%s/.config/recmd/history.db", home);
    if (n < 0 || n >= (int)sizeof(path)) {
        fprintf(stderr, "path too long\n");
        return NULL;
    }

    return path;
}

sqlite3 *db_init(void)
{
    sqlite3 *db;
    int rc = sqlite3_open(db_path(), &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "error opening the db: %s\n", sqlite3_errmsg(db));
        db_close(db);
    }

    const char *q =
        "CREATE TABLE IF NOT EXISTS history("
        "id        INTEGER PRIMARY KEY AUTOINCREMENT,"
        "command   TEXT,"
        "directory TEXT,"
        "exit_code INTEGER,"
        "ts        DATETIME DEFAULT CURRENT_TIMESTAMP);";

    char *errmsg;
    rc = sqlite3_exec(db, q, NULL, 0, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "error creating schema: %s\n", errmsg);
        sqlite3_free(errmsg);
        db_close(db);
        return NULL;
    }

    return db;
}

void db_close(sqlite3 *db)
{
    if (!db) return;
    int rc = sqlite3_close(db);
    if (rc != SQLITE_OK)
        fprintf(stderr, "database did not close cleanly (error: %d)\n", rc);
}

int db_record_command(sqlite3 *db, const char *cmd, const char *dir, int exit_code)
{
    const char *q = "INSERT INTO history(command, directory, exit_code) VALUES (?, ?, ?);";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, q, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "prepare failed: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, cmd,  -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, dir,  -1, SQLITE_STATIC);
    sqlite3_bind_int (stmt, 3, exit_code);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "step failed: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}

void db_result_free(db_result_t *result)
{
    if (!result) return;
    for (int i = 0; i < result->count; i++) {
        free(result->rows[i].command);
        free(result->rows[i].directory);
        free(result->rows[i].ts);
    }
    free(result->rows);
    free(result);
}

db_result_t *db_fetch_commands(sqlite3 *db, const char *cmd)
{
    const char *q =
        "SELECT DISTINCT command"
        " FROM history WHERE command LIKE ?";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, q, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "prepare failed: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    /* build the LIKE pattern with bound parameter — avoids mprintf */
    char pattern[512];
    snprintf(pattern, sizeof(pattern), "%%%s%%", cmd);
    sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_STATIC);

    db_result_t *result = calloc(1, sizeof(db_result_t));
    if (!result) {
        sqlite3_finalize(stmt);
        return NULL;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        db_row_t *tmp = realloc(result->rows, sizeof(db_row_t) * (result->count + 1));
        if (!tmp) {
            fprintf(stderr, "out of memory\n");
            db_result_free(result);
            sqlite3_finalize(stmt);
            return NULL;
        }
        result->rows = tmp;

        db_row_t *row = &result->rows[result->count];
        const char *col_cmd = (const char *)sqlite3_column_text(stmt, 0);
        const char *col_dir = (const char *)sqlite3_column_text(stmt, 1);
        const char *col_ts  = (const char *)sqlite3_column_text(stmt, 3);

        row->command   = strdup(col_cmd ? col_cmd : "");
        row->directory = strdup(col_dir ? col_dir : "");
        row->exit_code = sqlite3_column_int(stmt, 2);
        row->ts        = strdup(col_ts  ? col_ts  : "");

        if (!row->command || !row->directory || !row->ts) {
            fprintf(stderr, "out of memory\n");
            db_result_free(result);
            sqlite3_finalize(stmt);
            return NULL;
        }

        result->count++;
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "step failed: %s\n", sqlite3_errmsg(db));
        db_result_free(result);
        sqlite3_finalize(stmt);
        return NULL;
    }

    sqlite3_finalize(stmt);
    return result;
}
