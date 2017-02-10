select:
	cp ../libsdb.so ./libsdb.so ; cc select.c --std=c99 -o select -l:libsdb.so -Wl,-rpath '-Wl,$$ORIGIN'
clean:
	rm select
