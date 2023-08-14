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

## Custom Allocator

`glob_utf8` allocates and deallocates memory on each call because it needs to
to decode UTF-8. If you want to avoid that you can define your custom temporary
allocator.

```console
#include <stdio.h>

char temp[1024];
size_t temp_bump = 0;

void *temp_alloc(size_t size)
{
    if (temp_bump + size > sizeof(temp)) return NULL;
    void *result = temp + temp_bump;
    temp_bump += size;
    return result;
}

#define GLOB_IMPLEMENTATION
#define GLOB_MALLOC temp_alloc
#define GLOB_FREE(...)
#include "glob.h"

int main(void)
{
    const char *texts[] = { "main.c", "test.c", "index.js", "reset.css" };
    size_t n = sizeof(texts)/sizeof(texts[0]);
    for (size_t i = 0; i < n; ++i) {
        if (!glob_utf8("*.c", texts[i])) {
            printf("%s\n", texts[i]);
        }
        temp_bump = 0;
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
