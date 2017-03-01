# libsdb
Small key-value database library for embedded or lowmemory devices

|  OBJECT |   STATUS  |
|---------|-----------|
|Library|Done. Only FILE engine is available.|
|Interface| Done. |
|Documentation| Partially done. |
|Default engine|Not implemented.|
|File engine| Done. |
| Tests | Only integration tests for file engine. |


# API
DB API:

|Method|Explanation|Prototype|
|------|---------|-----------|
|insert|add new record|bool sdb_insert(sdb_dbo *db, const char *key, const char *value);|
|update|update existing record|bool sdb_update(sdb_dbo *db, const char *key, const char *value);|
|select|select existing record|const char *sdb_select(sdb_dbo *db, const char *key);|
|delete|remove existing record|bool sdb_delete(sdb_dbo *db, const char *key);|
|exist |Check record existence by returing non-negative size|ssize_t sdb_exist(sdb_dbo *db, const char *key);|

Access&utility API:

|Method|Explanation|Prototype|
|------|---------|-----------|
|      |open or create database|sdb_dbo *sdb_open(sdb_engine engine, void *params);|
|      |close database|void sdb_close(sdb_dbo *db);|
|      |set buffer size for next database from sdb_open() call|void sdb_tune(void *your_buffer, size_t your_buffer_size);|
|      |pass different your own memory management functions|void sdb_configure(void *(*y_malloc)(size_t size), void *(*y_calloc)(size_t nmemb, size_t size), void (*y_free)(void *ptr), void *(*y_realloc)(void *ptr, size_t size));|


# Overview

libsdb is configurable key-value database libraray for your own applications. You need to perform 3 steps to start using it:

1. Download & compile
2. Include libsdb.h to your project
3. Link your project with libsdb.so shared library

libsdb was made of view of KISS principle, portability, small memory usage. I'm planning (at this moment) to support 3 database engines:

1. MYSQL
2. Memory
3. Files

Currently, only file engine is implemented. Maybe if someone (or me) needs more - I'll implement all of them.

At this moment you can configure:

1. Memory management functions for whole library
2. Buffer size for each database. (default buffer size is defined in libsdb.h)

# Build

1. Download this repository via git
2. cd to downloaded directory
3. execute "make" command

# Getting started

Here is an example pseudocode to help you understand how easy you can use it.
```c
...
#include <libsdb.h>

...

{
    ...

    sdb_dbo *db = sdb_open(SDB_FILENO, "my_storage");
    const char *result = sdb_select(db, "mycoolkey");

    ...

}
```

# Contribution

Use github issues or pull requests for questions or contribution.

# Notes

1. You may need some kind of wrapper if you wish to use it on some different programming language then C.
2. If you are able to compile & link with library, but can't start your program, make sure that copy of libsdb.so file is located in appropriate path. Or just configure LD_LIBRARY_PATH.
