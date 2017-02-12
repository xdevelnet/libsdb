all:
	cp ../libsdb.so ./libsdb.so ; cc tests.c --std=c99 -o tests -l:libsdb.so -Wl,-rpath '-Wl,$$ORIGIN'
clean:
	rm select; rm libsdb.so
