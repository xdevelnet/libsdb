#ifndef LIBSDB_H
#define LIBSDB_H

#include <stdbool.h>

#define LIBSDB_MAXVALUE 65535

typedef enum { SDB_DEFAULT, SDB_FILENO } sdb_engine;

typedef struct sdb_dbo_s sdb_dbo;

extern signed int enomem_flag;
extern char *your_own_buffer;
extern size_t your_own_buffer_size;
extern ssize_t read_size_hook;

sdb_dbo *sdb_open(sdb_engine engine, void *params);
// Uses existing database or creates new one.
// Depends on selected engine, you may pass additional info to "params":
//     SDB_DEFAULT: not implemented yet
//     SDB_FILENO: you can pass string as full or relative directory path for file DB storage. If you pass NULL, default
//                 "sdb_storage" directory will be used.

void sdb_configure(void *(*y_malloc)(size_t size), void *(*y_calloc)(size_t nmemb, size_t size),
                   void (*y_free)(void *ptr), void *(*y_realloc)(void *ptr, size_t size));
// Pass your own malloc(), calloc(), free(), realloc() functions if you haven't them on your own system, or
// if you want to use different kind of memory management. It's not necessary to call this function.

void sdb_tune(void *your_buffer, size_t your_buffer_size);
// Pass your own buffer. Useful if 65535 maximum value size for your SELECT's aren't enough.

bool sdb_insert(sdb_dbo *db, const char *key, const char *value);
// Add new record. Returns false if record with such key is already exist, or we haven't enough memory.

bool sdb_update(sdb_dbo *db, const char *key, const char *value);
// Update existing record. Returns false if record with such key is not exist, or we haven't enough memory.

const char *sdb_select(sdb_dbo *db, const char *key);
// Select record. Returns NULL if record with such key is not exist, or we haven't enough memory.

bool sdb_delete(sdb_dbo *db, const char *key);
// Remove record. Returns false if record with such key is not exist, or we haven't enough memory.

ssize_t sdb_exist(sdb_dbo *db, const char *key);
// Check if record exist. Returns -1 if record with such key is not exist, otherwise returns length of value.

#endif