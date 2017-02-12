cmake_minimum_required(VERSION 3.1)
project(libsdb)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        libsdb.c
        libsdb.h libsdb_fileno.h tests/select.c)

add_executable(libsdb ${SOURCE_FILES})