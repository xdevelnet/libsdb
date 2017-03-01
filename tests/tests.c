#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h> // I could not use it, but use fseek() instead of stat(). But who cares?
#include "../libsdb.h"

#define eprintf(...) fprintf (stderr, __VA_ARGS__) // KANONI4NO

#define WORKING_DIR "test_data"
#define strizeof(a) sizeof(a)-1
#define TEST_BUFFER_SIZE 1024*3000

#define INSERT_TEST_FILE "insert_test"
#define UPDATE_TEST_FILE "update_test"
#define SELECT_TEST_FILE "select_test"
#define DELETE_TEST_FILE "delete_test"
#define EXIST_TEST_FILE "exist_test"

char *buffer;

/*
 * TEST CASES:
 * 
 * 1) Prepare string. Execute "insert" method. Check if file exist with correct rights and content is equal to string.
 * 2) Create file with some contents. Prepare string. Execute "update" method.
 *    Check if file exist with correct rights and content is equal to string.
 * 3) Create file with some contents without '\0' (so, that should be string). Execute "select" method. Compare strings.
 * 4) Create file with any contents and appropriate rights. Execute "delete" method. Check if file NOT exist.
 * 5) Create non-empty file. Execute "exist" method. Check if return value is equal to file size.
 * 
 */

const char example_string[] = "ᚠᛇᚻ᛫ᛒᛦᚦ᛫ᚠᚱᚩᚠᚢᚱ᛫ᚠᛁᚱᚪ᛫ᚷᛖᚻᚹᛦᛚᚳᚢᛗ\nAn preost wes on leoden, Laȝamon was ihoten\n"
	"Sîne klâwen durh die wolken sint geslagen\nΤη γλώσσα μου έδωσαν ελληνική\nНа берегу пустынных волн\n"
	"ვეპხის ტყაოსანი შოთა რუსთაველი\n我能吞下玻璃而不伤身体\n나는 유리를 먹을 수 있어요. 그래도 아프지 않아요a\n"
	"私はガラスを食べられます。それは私を傷つけません\nЯ можу їсти шкло, й воно мені не пошкодить. Я точно не людина.";

#include "test_preparations.h"

int main() {
	if (chdir(WORKING_DIR) < 0) {
		eprintf("Can't change directory to %s. You should run tests from right place\n", WORKING_DIR);
		return EXIT_FAILURE;
	}
	if (test_prep() == false) {
		eprintf("Test preparations was completely failed");
		return EXIT_FAILURE;
	}
	if (chdir("..") < 0) {
		perror(NULL);
		return EXIT_FAILURE;
	}
	sdb_tune(65536 * 7);
	sdb_dbo *db = sdb_open(SDB_FILENO, WORKING_DIR);
	if (db == NULL) {
		eprintf("Open failed.\n");
		return EXIT_FAILURE;
	}

	// CASE 1

	if (sdb_insert(db, INSERT_TEST_FILE, example_string) == false) {
		eprintf("Insert failed.\n");
		return EXIT_FAILURE;
	}
	int fd = open(WORKING_DIR "/" INSERT_TEST_FILE, O_RDONLY);
	if (fd < 0) {
		eprintf("Failed to open %s\n", WORKING_DIR "/" INSERT_TEST_FILE);
	}
	ssize_t got = read(fd, buffer, TEST_BUFFER_SIZE);
	if (got <= 0) return EXIT_FAILURE;
	if (strizeof(example_string) != (size_t) got) {
		eprintf("Insert was succesful, but sizes aren't equal. U WOT M9\n");
		return EXIT_FAILURE;
	}
	if (memcmp(example_string, buffer, strizeof(example_string)) != 0) {
		eprintf("Insert was successful, but strings aren't equal. Welp.\n");
		return EXIT_FAILURE;
	}
	close(fd);
	eprintf("Insert was successful\n");

	// CASE 2

	if (sdb_update(db, UPDATE_TEST_FILE, example_string) == false) {
		eprintf("Update failed.\n");
		return EXIT_FAILURE;
	}
	fd = open(WORKING_DIR "/" UPDATE_TEST_FILE, O_RDONLY);
	if (fd < 0) {
		eprintf("Failed to open %s\n", WORKING_DIR "/" UPDATE_TEST_FILE);
	}
	got = read(fd, buffer, TEST_BUFFER_SIZE);
	if (got <= 0) return EXIT_FAILURE;
	if (strizeof(example_string) != (size_t) got) {
		eprintf("Update was successful, but sizes aren't equal. Get rekt m8.\n");
		return EXIT_FAILURE;
	}
	if (memcmp(example_string, buffer, strizeof(example_string)) != 0) {
		eprintf("Update was successful, but strings aren't equal. Welp.\n");
		return EXIT_FAILURE;
	}
	close(fd);
	eprintf("Update was successful\n");

	// CASE 3

	const char *result = sdb_select(db, SELECT_TEST_FILE);
	
	if (result == NULL) {
		eprintf("Select failed.\n");
		return EXIT_FAILURE;
	}
	if (strcmp(result, example_string) != 0) {
		eprintf("Select was successful, but strings aren't equal.\n");
		return EXIT_FAILURE;
	}
	eprintf("Select was successful\n");

	// CASE 4

	if (sdb_delete(db, DELETE_TEST_FILE) != true) {
		eprintf("Select failed.\n");
		return EXIT_FAILURE;
	}
	if (access(DELETE_TEST_FILE, F_OK) == 0) {
		eprintf("Delete was successful, but file is still exist. Wait, what?\n");
		return EXIT_FAILURE;
	}
	eprintf("Delete was successful\n");
	
	// CASE 5
	
	ssize_t signed_size_result = sdb_exist(db, EXIST_TEST_FILE);
	
	if (signed_size_result <= 0) {
		eprintf("Exist failed.\n");
		return EXIT_FAILURE;
	}
	struct stat st; // stackoverflow-driven programming in action!
	// (actually no, I just don't want to spend alot of time)
	if (stat(WORKING_DIR "/" EXIST_TEST_FILE, &st) != 0) {
		eprintf("Exist was successful, but we can't retreive stat() info from file. WTF?\n");
		return EXIT_FAILURE;
	}
	if (signed_size_result != (ssize_t) st.st_size) {
		eprintf("Exist was successful, but we sizes aren't equal. WTF?\n");
		eprintf("st.size: %zd ssize_t result: %zd\n", st.st_size, signed_size_result);
		return EXIT_FAILURE;
	}
	eprintf("Exist was successful\n");
	free(buffer);
	sdb_close(db);
	
	return EXIT_SUCCESS;
}
