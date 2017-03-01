library:
	cc -c libsdb.c -o libsdb.o -fPIC --std=c99 -Wall -Werror -O2
	cc libsdb.o -o libsdb.so --std=c99 -shared  -Wall -Werror -O2
tests:
	false;
clean:
	rm libsdb.o
	rm libsdb.so
