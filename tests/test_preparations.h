#define URANDOM_PATH "/dev/urandom"

bool test_prep() {
	buffer = malloc(TEST_BUFFER_SIZE);
	if (buffer == NULL) {
		eprintf("memory allocation failure\n");
		return false;
	}

	unlink(INSERT_TEST_FILE);
	unlink(UPDATE_TEST_FILE);
	unlink(SELECT_TEST_FILE);
	unlink(DELETE_TEST_FILE);
	unlink(EXIST_TEST_FILE);

	int trash_source_fd = open(URANDOM_PATH, O_RDONLY);
	if (trash_source_fd < 0) {
		perror(NULL);
		return false;
	}
	if (read(trash_source_fd, buffer, TEST_BUFFER_SIZE) <= 0) return false;

	// CASE 2

	int fd = open(UPDATE_TEST_FILE, O_WRONLY | O_CREAT | O_EXCL, 0666);
	if (fd < 0) {
		perror(NULL);
		close(trash_source_fd);
		return false;
	}
	if (write(fd, buffer, TEST_BUFFER_SIZE) <= 0) return false;
	close(fd);

	if (read(trash_source_fd, buffer, TEST_BUFFER_SIZE) <= 0) return false;

	// CASE 4

	fd = open(DELETE_TEST_FILE, O_WRONLY | O_CREAT | O_EXCL, 0666);
	if (fd < 0) {
		perror(NULL);
		close(trash_source_fd);
		return false;
	}
	if (write(fd, buffer, TEST_BUFFER_SIZE) <= 0) return false;
	close(fd);

	if (read(trash_source_fd, buffer, TEST_BUFFER_SIZE) <= 0) return false;

	// CASE 5

	fd = open(EXIST_TEST_FILE, O_WRONLY | O_CREAT | O_EXCL, 0666);
	if (fd < 0) {
		perror(NULL);
		close(trash_source_fd);
		return false;
	}
	if (write(fd, buffer, TEST_BUFFER_SIZE) <= 0) return false;
	close(fd);

	close(trash_source_fd);

	// CASE 3

	fd = open(SELECT_TEST_FILE, O_WRONLY | O_CREAT | O_EXCL, 0666);
	if (fd < 0) {
		perror(NULL);
		close(trash_source_fd);
		return false;
	}
	if (write(fd, example_string, strizeof(example_string)) < 0) return false;
	close(fd);

	return true;
}
