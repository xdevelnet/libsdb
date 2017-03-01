# libsdb
Small key-value database engine library for embedded or lowmemory devices

|  OBJECT |   STATUS  |
|---------|-----------|
|Library|Done. Only FILE engine is available|
|Interface| Done. |
|Documentation| Partially done |
|Default engine|Not implemented.|
|File engine| Done. |
| Tests | Only integration tests for file engine done |


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
