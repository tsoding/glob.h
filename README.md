# Simple Header-Only Implementation of Glob Matching

## Testing

```console
$ ./build.sh
$ ./test_glob
```
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
