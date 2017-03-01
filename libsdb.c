/*
 * Copyright (c) 2017, Xdevelnet (xdevelnet at xdevelnet dot org)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
size_t omit_buffer_size = 0;

void *(*my_malloc)(size_t size) = malloc; // "I'll be my own successor!" (c)
void *(*my_calloc)(size_t nmemb, size_t size) = calloc;
void (*my_free)(void *ptr) = free;
void *(*my_realloc)(void *ptr, size_t size) = realloc;

#include "libsdb_fileno.h"

sdb_dbo *sdb_open(sdb_engine engine, void *params) {
	switch (engine) {
		case SDB_DEFAULT: return false;
		case SDB_FILENO: return sdb_open_fileno(engine, params);
		case SDB_MYSQL: return false;
		default: return false;
	}
}

void sdb_configure(void *(*y_malloc)(size_t size), void *(*y_calloc)(size_t nmemb, size_t size),
                   void (*y_free)(void *ptr), void *(*y_realloc)(void *ptr, size_t size)) {
	if (y_malloc != NULL and y_calloc != NULL and y_free != NULL and y_realloc != NULL) {
		my_malloc = y_malloc, my_calloc = y_calloc, my_free = y_free, my_realloc = y_realloc; // Did you get it? :D
	}
}

void sdb_tune(size_t your_buffer_size) {
	if (your_buffer_size > 0) omit_buffer_size = your_buffer_size;
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

void sdb_close(sdb_dbo *db) {
	if (db == NULL) return;
	switch (db->engine) {
		case SDB_DEFAULT: return;
		case SDB_FILENO: return sdb_close_fileno(db);
		case SDB_MYSQL: return;
		default: return;
	}
}