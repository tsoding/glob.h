#!/bin/sh

set -xe

# Source-based Code Coverage with Clang: https://clang.llvm.org/docs/SourceBasedCodeCoverage.html

clang -Wall -Wextra -Wswitch-enum -fprofile-instr-generate -fcoverage-mapping -ggdb -o test_glob test_glob.c ConvertUTF.c
./test_glob
llvm-profdata merge -sparse ./default.profraw -o default.profdata
llvm-cov show ./test_glob -instr-profile=default.profdata glob.h
