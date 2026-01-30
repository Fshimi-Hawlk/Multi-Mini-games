#ifndef DATABASE_H
#define DATABASE_H

#include "utils/common.h"
#include <sqlite3.h>

bool db_init(void);
bool db_userExists(const char *username);
bool db_checkPassword(const char *username, const char *password);
bool db_registerUser(const char *username, const char *password);
void db_close(void);

#endif