#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libsdb.h"

#define eprintf(...) fprintf (stderr, __VA_ARGS__) // KANONI4NO

#define WORKING_DIR "example_data"
#define strizeof(a) sizeof(a)-1
#define TEST_BUFFER_SIZE 1024*3000

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

#define INSERT_TEST_FILE "insert_test"

int main() {
	if (chdir(WORKING_DIR) < 0) {
		eprintf("Can't change directory to %s. You should run tests from right place\n", WORKING_DIR);
		return EXIT_FAILURE;
	}
	char *buffer = malloc(TEST_BUFFER_SIZE);
	if (buffer == NULL) return EXIT_FAILURE;
	unlink(INSERT_TEST_FILE);
	chdir("..");
	
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
		eprintf("Insert was succes, but sizes aren't equal. U got rekt m8.\n");
		return EXIT_FAILURE;
	}
	if (memcmp(example_string, buffer, strizeof(example_string)) != 0) {
		eprintf("Insert was succes, but strings aren't equal. Welp.\n");
		return EXIT_FAILURE;
	}
	close(fd);
	
	// CASE 2
	
	return EXIT_SUCCESS;
}
