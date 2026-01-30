// database.c
#include "database.h"
#include <openssl/evp.h>
#include <openssl/rand.h>

static sqlite3 *db = NULL;

static void pbkdf2_hash(const char *password, const unsigned char *salt, unsigned char *out) {
    PKCS5_PBKDF2_HMAC(password, -1, salt, 16, 100000, EVP_sha256(), 32, out);
}

bool db_init(void) {
    if (sqlite3_open("chat_users.db", &db) != SQLITE_OK) {
        log_error("Cannot open database");
        return false;
    }

    const char *sql = \
        "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "username TEXT UNIQUE NOT NULL,"
            "password_hash BLOB NOT NULL,"
            "salt BLOB NOT NULL"
        ");";

    char *err = NULL;
    if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK) {
        log_error("SQL error: %s", err);
        sqlite3_free(err);
        return false;
    }
    return true;
}

bool db_userExists(const char *username) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT 1 FROM users WHERE username = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    int result = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return result;
}

bool db_checkPassword(const char *username, const char *password) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT password_hash, salt FROM users WHERE username = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return false;
    }

    const unsigned char *stored = sqlite3_column_blob(stmt, 0);
    const unsigned char *salt   = sqlite3_column_blob(stmt, 1);

    unsigned char computed[32];
    pbkdf2_hash(password, salt, computed);

    bool ok = (memcmp(stored, computed, 32) == 0);
    sqlite3_finalize(stmt);
    return ok;
}

bool db_registerUser(const char *username, const char *password) {
    if (db_userExists(username)) return false;

    unsigned char salt[16];
    RAND_bytes(salt, 16);

    unsigned char hash[32];
    pbkdf2_hash(password, salt, hash);

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO users (username, password_hash, salt) VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 2, hash, 32, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 3, salt, 16, SQLITE_STATIC);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

void db_close(void) { 
    if (db) sqlite3_close(db); 
}