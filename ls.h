#ifndef LS_H
#define LS_H

#include "db.h"

void ls_command(int argc, char **argv);

void print_commands(const db_result_t *result);

#endif
