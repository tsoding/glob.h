# Simple Header-Only Implementation of Glob Matching

## Usage

```c
#include <stdio.h>
#include <string.h>
#define GLOB_IMPLEMENTATION
#include "glob.h"

int main(void)
{
    if (!glob_utf8("*.c", "main.c")) {
        printf("OK\n");
    } else {
        printf("FAIL\n");
    }
    return 0;
}
```

## Testing

```console
$ ./build.sh
$ ./test_glob
```
## Coverage

Very useful to see what's not tested yet. Requires [clang](https://clang.llvm.org/).

```console
$ ./coverage.sh
```
