#!/bin/sh

set -xe

clang -Wall -Wextra -Wswitch-enum -ggdb -o test_glob test_glob.c
./test_glob
