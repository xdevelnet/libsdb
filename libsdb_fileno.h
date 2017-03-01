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

#ifndef LIBSDB_LIBSDB_FILENO_H
#define LIBSDB_LIBSDB_FILENO_H

#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

#define DEFAULT_STORAGE_DIRECTORY "sdb_storage"

//#define LIBSDB_DEBUG

#ifdef LIBSDB_DEBUG
	#include <stdio.h>
#endif

inline static bool prepare_path(const char *dir, const char *file, char *buffer) {
	size_t dir_len = strlen(dir);
	size_t file_len = strlen(file);

	if (file_len > NAME_MAX or file_len + dir_len + 1 > PATH_MAX) return false;

	char *fly = memcpy(buffer, dir, dir_len) + dir_len;
	*fly = '/';
	fly++;
	memcpy(fly, file, file_len + 1);

	return true;
}

bool sdb_insert_fileno(sdb_dbo *db, const char *key, const char *value) {
	char path_buffer[PATH_MAX] = "";
	prepare_path(db->dataset, key, path_buffer);

#ifdef LIBSDB_DEBUG
	fprintf(stderr, "Requested INSERT to path: %s key: %s value: %s\n", path_buffer, key, value);
#endif

	int fd = open(path_buffer, O_WRONLY | O_CREAT | O_EXCL, 0666);

#ifdef LIBSDB_DEBUG
	fprintf(stderr, "Opening file. fd: %d errno: %d\n", fd, errno);
#endif

	if (fd < 0) {
		if (errno == ENOMEM) enomem_flag = 1;
		return false;
	}
	errno = 0;

	size_t value_length = strlen(value);
	ssize_t got = write(fd, value, value_length);

#ifdef LIBSDB_DEBUG
	fprintf(stderr, "Writed contents to file. requested: %zu got: %zd\n", value_length, got);
#endif

	if (got < 0) {
		if (errno == ENOMEM or errno == EFBIG or errno == ENOSPC) enomem_flag = 1;
		close(fd);
		return false;
	}

	// You may wondering why "not enough space" checking is so shitty.
	// That's because in practice filesystem may write less amount of bytes then requested without setting errno. Whoa!
	//
	// Wanna make a test?
	// Create small ext2 filesystem. Let's use 1MB size. Now create a single file and try to write 1.5MB data.
	// write() will return something around 1MB. errno is still 0! What?!
	// Ok, now more - TRY TO CALL write() WITH SUPER SMALL AMOUNT OF BYTES: ssize_t got = write(fd, "aaa", 3);
	// And that call will return... 3. Well fucking done, Linux! Well fucking done.

	// TODO: research O_DIRECT

	if (errno == ENOMEM or errno == EFBIG or errno == ENOSPC or (size_t) got < value_length) enomem_flag = 1;
	close(fd);

	return true;
}

bool sdb_update_fileno(sdb_dbo *db, const char *key, const char *value) {
	char path_buffer[PATH_MAX] = "";
	prepare_path(db->dataset, key, path_buffer);

#ifdef LIBSDB_DEBUG
	fprintf(stderr, "Requested UPDATE to path: %s key: %s value: %s\n", path_buffer, key, value);
#endif

	int fd = open(path_buffer, O_WRONLY | O_TRUNC);

#ifdef LIBSDB_DEBUG
	fprintf(stderr, "Opening and truncating file. fd: %d errno: %d\n", fd, errno);
#endif

	if (fd < 0) {
		if (errno == ENOMEM) enomem_flag = 1;
		return false;
	}
	errno = 0;

	size_t value_length = strlen(value);
	ssize_t got = write(fd, value, value_length);

#ifdef LIBSDB_DEBUG
	fprintf(stderr, "Writed contents to file. requested: %zu got: %zd\n", value_length, got);
#endif

	if (got < 0) {
		if (errno == ENOMEM or errno == EFBIG or errno == ENOSPC) enomem_flag = 1;
		close(fd);
		return false;
	}
	if (errno == ENOMEM or errno == EFBIG or errno == ENOSPC or (size_t) got < value_length) enomem_flag = 1;
	close(fd);

	return true;
}

const char *sdb_select_fileno(sdb_dbo *db, const char *key) {
	char path_buffer[PATH_MAX] = "";
	prepare_path(db->dataset, key, path_buffer);

	int fd = open(path_buffer, O_RDONLY);
	if (fd < 0) {
		if (errno == ENOMEM) enomem_flag = 1;
		return NULL;
	}
	errno = 0;

	ssize_t got = read(fd, db->data, db->dataset_total);

	if (got < 0) {
		close(fd);
		return false;
	}
	if (errno == ENOMEM or errno == EFBIG or errno == ENOSPC) enomem_flag = 1;
	close(fd);

	((char *)&db->data)[db->dataset_total] = '\0'; // WRECKED!
	read_size_hook = got;

	return db->data;
}

bool sdb_delete_fileno(sdb_dbo *db, const char *key) {
	char path_buffer[PATH_MAX] = "";
	prepare_path(db->dataset, key, path_buffer);

	if (unlink(path_buffer) < 0) return false;
	return true;
}

ssize_t sdb_exist_fileno(sdb_dbo *db, const char *key) {
	char path_buffer[PATH_MAX] = "";
	prepare_path(db->dataset, key, path_buffer);

	if (access(path_buffer, R_OK | W_OK) < 0) return false;
	struct stat st;
	if (stat(path_buffer, &st) < 0) return false;
	return (ssize_t) st.st_size;
}

sdb_dbo *sdb_open_fileno(sdb_engine engine, void *params) {
	if (engine != SDB_FILENO) return false;
	size_t mem4str = 0;
	size_t storagesize;
	if (omit_buffer_size > 0) storagesize = omit_buffer_size; else storagesize = LIBSDB_MAXVALUE;
	omit_buffer_size = 0;
	if (params != NULL) mem4str = strlen(params) + 1;
	// I could use strdup() for "params" and don't give a fuck, but I prefer to use my own savvy.
	sdb_dbo *source = my_calloc(sizeof(sdb_dbo) + mem4str + storagesize, sizeof(char));
	if (source == NULL) return NULL;
	if (params == NULL) source->dataset = DEFAULT_STORAGE_DIRECTORY;
	else source->dataset = memcpy(sizeof(sdb_dbo) + (char *) source, params, mem4str);
	// Write me an email if you need explanation about that line above. No, actually don't do that.

	source->data = mem4str + sizeof(sdb_dbo) + (char *) source;
	source->dataset_total = storagesize;

	if (access(source->dataset, W_OK) == 0) {
		source->defun.p_sdb_insert = sdb_insert_fileno;
		source->defun.p_sdb_update = sdb_update_fileno;
		source->defun.p_sdb_select = sdb_select_fileno;
		source->defun.p_sdb_delete = sdb_delete_fileno;
		source->defun.p_sdb_exist = sdb_exist_fileno;
		source->engine = engine;
		return source;
	} else {
		my_free(source);
		return NULL;
	}
}

void sdb_close_fileno(sdb_dbo *db) {
	// Don't want to check if db == NULL. I suppose that's already done before.
	return my_free(db);
}

#endif //LIBSDB_LIBSDB_FILENO_H
