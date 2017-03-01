all:
	cp ../libsdb.so ./libsdb.so ; cc tests.c --std=c99 -o tests -O2 -Wall -Werror -l:libsdb.so -Wl,-rpath '-Wl,$$ORIGIN'
clean:
	rm tests libsdb.so; rm -Rf test_data/*
