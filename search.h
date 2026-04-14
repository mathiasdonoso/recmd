#ifndef SEARCH_H
#define SEARCH_H

#include "db.h"

void search_command(int argc, char **argv);

void print_commands(const db_result_t *result);

#endif
