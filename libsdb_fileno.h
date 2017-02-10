#ifndef LIBSDB_LIBSDB_FILENO_H
#define LIBSDB_LIBSDB_FILENO_H

#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>

inline bool prepare_path(const char *dir, const char *file, char *buffer) {
	size_t dir_len = strlen(dir);
	size_t file_len = strlen(file);

	if (file_len > NAME_MAX or file_len + dir_len + 1 > PATH_MAX) return false;

	char *fly = memcpy(buffer, dir, dir_len) + dir_len;
	*fly = '/';
	fly++;
	memcpy(fly, file, file_len);

	return true;
}

bool sdb_insert_fileno(sdb_dbo *db, const char *key, const char *value) {
	char path_buffer[PATH_MAX] = "";
	prepare_path(db->dataset, key, path_buffer);

	int fd = open(path_buffer, O_WRONLY);
	if (fd < 0) {
		if (errno == ENOMEM) enomem_flag = 1;
		return false;
	}
	errno = 0;

	size_t value_length = strlen(value);
	if (write(fd, value, value_length) < 0) {
		close(fd);
		return false;
	}
	if (errno == ENOMEM or errno == EFBIG or errno == ENOSPC) enomem_flag = 1;
	close(fd);

	return true;
}

bool sdb_update_fileno(sdb_dbo *db, const char *key, const char *value) {
	return false;
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
	static char buffer[LIBSDB_MAXVALUE];

	ssize_t got = read(fd, buffer, LIBSDB_MAXVALUE - 1);
	if (got < 0) {
		close(fd);
		return false;
	}
	if (errno == ENOMEM or errno == EFBIG or errno == ENOSPC) enomem_flag = 1;
	close(fd);

	buffer[got] = '\0';

	return buffer;
}

bool sdb_delete_fileno(sdb_dbo *db, const char *key) {
	return false;
}

ssize_t sdb_exist_fileno(sdb_dbo *db, const char *key) {
	return false;
}

sdb_dbo *sdb_open_fileno(sdb_engine engine, void *params) {
	if (engine != SDB_FILENO) return false;

	sdb_dbo *source = calloc(sizeof(sdb_dbo), sizeof(char));
	if (source == NULL) return NULL;

	if (params != NULL) source->dataset = params; else
		source->dataset = "sdb_storage";

	if (access(source->dataset, W_OK) == 0) {
		source->defun.p_sdb_insert = sdb_insert_fileno;
		source->defun.p_sdb_update = sdb_update_fileno;
		source->defun.p_sdb_select = sdb_select_fileno;
		source->defun.p_sdb_delete = sdb_delete_fileno;
		source->defun.p_sdb_exist = sdb_exist_fileno;
		return source;
	} else {
		my_free(source);
		return NULL;
	}
}

#endif //LIBSDB_LIBSDB_FILENO_H
