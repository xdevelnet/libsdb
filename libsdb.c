#define _POSIX_SOURCE

#include <sys/types.h>
#include <stddef.h>
#include <iso646.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "libsdb.h"

typedef struct {
	bool (*p_sdb_insert)(sdb_dbo *db, const char *key, const char *value);
	bool (*p_sdb_update)(sdb_dbo *db, const char *key, const char *value);
	const char *(*p_sdb_select)(sdb_dbo *db, const char *key);
	bool (*p_sdb_delete)(sdb_dbo *db, const char *key);
	ssize_t (*p_sdb_exist)(sdb_dbo *db, const char *key);
} sdb_defun;

struct sdb_dbo_s {
	sdb_engine engine;
	void *dataset;
	void *data;
	size_t dataset_used; // amount of records
	size_t dataset_total; // same
	size_t data_used; // amount of bytes
	size_t data_total; // same
	sdb_defun defun;
};

signed int enomem_flag = 0; // check this flag if library function fails because of
ssize_t read_size_hook = 0; // check how much bytes has been transferred after library call

void *(*my_malloc)(size_t size) = malloc; // "I'll be my own successor!" (c)
void *(*my_calloc)(size_t nmemb, size_t size) = calloc;
void (*my_free)(void *ptr) = free;
void *(*my_realloc)(void *ptr, size_t size) = realloc;

#include "libsdb_fileno.h"

sdb_dbo *sdb_open(sdb_engine engine, void *params) {
	switch (engine) {
		case SDB_DEFAULT: return false;
		case SDB_FILENO: return sdb_open_fileno(engine, params);
		default: return false;
	}
}

void sdb_configure(void *(*y_malloc)(size_t size), void *(*y_calloc)(size_t nmemb, size_t size),
                   void (*y_free)(void *ptr), void *(*y_realloc)(void *ptr, size_t size)) {
	if (y_malloc != NULL and y_calloc != NULL and y_free != NULL and y_realloc != NULL) {
		my_malloc = y_malloc, my_calloc = y_calloc, my_free = y_free, my_realloc = y_realloc; // Did you get it? :D
	}
}

char *your_own_buffer = NULL;
size_t your_own_buffer_size = 0;

void sdb_tune(void *your_buffer, size_t your_buffer_size) {
	if (your_buffer != NULL and your_own_buffer_size > 0) {
		your_own_buffer = your_buffer;
		your_own_buffer_size = your_buffer_size;
	}
}

bool sdb_insert(sdb_dbo *db, const char *key, const char *value) {
	return db->defun.p_sdb_insert(db, key, value);
}

bool sdb_update(sdb_dbo *db, const char *key, const char *value) {
	return db->defun.p_sdb_update(db, key, value);
}

const char *sdb_select(sdb_dbo *db, const char *key) {
	return db->defun.p_sdb_select(db, key);
}

bool sdb_delete(sdb_dbo *db, const char *key) {
	return db->defun.p_sdb_delete(db, key);
}

ssize_t sdb_exist(sdb_dbo *db, const char *key) {
	return db->defun.p_sdb_exist(db, key);
}