# Simple Header-Only Implementation of Glob Matching

## Usage

```c
#define GLOB_IMPLEMENTATION
#include "glob.h"

// Your custom UTF8 decode. Check test_glob.c for an example of that.
uint32_t *decode_utf8(const char *message);

int main(void)
{
    if (glob(decode_utf8("*.c"), decode_utf8("main.c")) == GLOB_MATCHED) {
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
