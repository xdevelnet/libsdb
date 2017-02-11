# libsdb
Small key-value database engine library for embedded or lowmemory devices

|  ITEM |   STATUS  |
|-------|-----------|
|Library|In development|
|Interface| Done. Small changes may be possible |
|Documentation| Partially done|
|Default engine|Not implemented yet|
|File engine| Prototyping |


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
