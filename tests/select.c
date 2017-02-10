#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "../libsdb.h"

int main() {
	sdb_dbo *db = sdb_open(SDB_FILENO, "example_data");
	if (db == NULL) return EXIT_FAILURE;

	const char *str = sdb_select(db, "blabla");
	if (str != NULL) printf("%s\n", str);
	return EXIT_SUCCESS;
}
