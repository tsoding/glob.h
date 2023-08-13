#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "ConvertUTF.h"
#define GLOB_IMPLEMENTATION
#include "glob.h"

uint32_t *decode_utf8(const char *message)
{
    size_t n = strlen(message);
    uint32_t *out = malloc(sizeof(*out)*(n + 1));
    assert(out != NULL && "Buy more RAM lol");
    memset(out, 0, sizeof(*out)*(n + 1));
    uint32_t *out_end = out;

    ConversionResult result = ConvertUTF8toUTF32(
                                  (const UTF8**) &message, (const UTF8*) (message + n),
                                  (UTF32**) &out_end, (UTF32*) out + n, 0);
    switch (result) {
        case conversionOK: return out;
        case sourceExhausted: {
            free(out);
            fprintf(stderr, "ERROR: partial character in source, but hit end");
            return NULL;
        }
        break;
        case targetExhausted: assert(0 && "unreachable");
        case sourceIllegal: {
            free(out);
            fprintf(stderr, "ERROR: source sequence is illegal/malformed");
            return NULL;
        } break;
    }
    assert(0 && "unreachable");
}

void check_glob_located(const char *file, int line, const char *pattern, const char *text, Glob_Result expected)
{
    uint32_t *pattern_utf32 = decode_utf8(pattern);
    if (pattern_utf32 == NULL) exit(1);
    uint32_t *text_utf32 = decode_utf8(text);
    if (text_utf32 == NULL) exit(1);

    Glob_Result actual = glob(pattern_utf32, text_utf32);
    printf("%12s <=> %-12s => %s\n", pattern, text, glob_result_display(actual));
    if (actual != expected) {
        printf("%s:%d: FAILURE! Expected %s", file, line, glob_result_display(expected));
        exit(1);
    }
}

#define check_glob(pattern, text, expected) check_glob_located(__FILE__, __LINE__, pattern, text, expected)

int main(void)
{
    check_glob("main.?", "main.c", GLOB_MATCHED);
    check_glob("index.?", "main.c", GLOB_UNMATCHED);
    printf("\n");
    check_glob("?at", "Cat", GLOB_MATCHED);
    check_glob("?at", "cat", GLOB_MATCHED);
    check_glob("?at", "Bat", GLOB_MATCHED);
    check_glob("?at", "bat", GLOB_MATCHED);
    check_glob("?at", "at", GLOB_UNMATCHED);
    printf("\n");
    check_glob("*", "main.c", GLOB_MATCHED);
    check_glob("*.c", "main.c", GLOB_MATCHED);
    check_glob("*.c", "index.c", GLOB_MATCHED);
    check_glob("*.c", "test.c", GLOB_MATCHED);
    check_glob("*.js", "main.c", GLOB_UNMATCHED);
    printf("\n");
    check_glob("Law*", "Law", GLOB_MATCHED);
    check_glob("Law*", "Laws", GLOB_MATCHED);
    check_glob("Law*", "Lawyer", GLOB_MATCHED);
    check_glob("Law*", "GrokLaw", GLOB_UNMATCHED);
    check_glob("Law*", "La", GLOB_UNMATCHED);
    check_glob("Law*", "aw", GLOB_UNMATCHED);
    printf("\n");
    check_glob("*Law*", "Law", GLOB_MATCHED);
    check_glob("*Law*", "GrokLaw", GLOB_MATCHED);
    check_glob("*Law*", "Lawyer", GLOB_MATCHED);
    check_glob("*Law*", "La", GLOB_UNMATCHED);
    check_glob("*Law*", "aw", GLOB_UNMATCHED);
    printf("\n");
    check_glob("*.[abc]", "main.a", GLOB_MATCHED);
    check_glob("*.[abc]", "main.b", GLOB_MATCHED);
    check_glob("*.[abc]", "main.c", GLOB_MATCHED);
    check_glob("*.[abc]", "main.d", GLOB_UNMATCHED);
    check_glob("*.[abc", "main.a", GLOB_SYNTAX_ERROR);
    printf("\n");
    check_glob("[CB]at", "Cat", GLOB_MATCHED);
    check_glob("[CB]at", "Bat", GLOB_MATCHED);
    check_glob("[CB]at", "cat", GLOB_UNMATCHED);
    check_glob("[CB]at", "bat", GLOB_UNMATCHED);
    check_glob("[CB]at", "CBat", GLOB_UNMATCHED);
    printf("\n");
    check_glob("[][!]", "]", GLOB_MATCHED);
    check_glob("[][!]", "[", GLOB_MATCHED);
    check_glob("[][!]", "!", GLOB_MATCHED);
    check_glob("[][!]", "a", GLOB_UNMATCHED);
    check_glob("[][!", "]", GLOB_SYNTAX_ERROR);
    printf("\n");
    check_glob("Letter[0-9]", "Letter0", GLOB_MATCHED);
    check_glob("Letter[0-9]", "Letter1", GLOB_MATCHED);
    check_glob("Letter[0-9]", "Letter2", GLOB_MATCHED);
    check_glob("Letter[0-9]", "Letter9", GLOB_MATCHED);
    printf("\n");
    check_glob("Letter[0-9]", "Letters", GLOB_UNMATCHED);
    check_glob("Letter[0-9]", "Letter", GLOB_UNMATCHED);
    check_glob("Letter[0-9]", "Letter10", GLOB_UNMATCHED);
    printf("\n");
    check_glob("[A-Fa-f0-9]", "A", GLOB_MATCHED);
    check_glob("[A-Fa-f0-9]", "a", GLOB_MATCHED);
    check_glob("[A-Fa-f0-9]", "B", GLOB_MATCHED);
    check_glob("[A-Fa-f0-9]", "b", GLOB_MATCHED);
    check_glob("[A-Fa-f0-9]", "0", GLOB_MATCHED);
    check_glob("[A-Fa-f0-9]", "-", GLOB_UNMATCHED);
    printf("\n");
    check_glob("[]-]", "]", GLOB_MATCHED);
    check_glob("[]-]", "-", GLOB_MATCHED);
    check_glob("[]-]", "a", GLOB_UNMATCHED);
    printf("\n");
    check_glob("[--0]", "-", GLOB_MATCHED);
    check_glob("[--0]", ".", GLOB_MATCHED);
    check_glob("[--0]", "0", GLOB_MATCHED);
    check_glob("[--0]", "/", GLOB_MATCHED);
    check_glob("[--0]", "a", GLOB_UNMATCHED);
    printf("\n");
    check_glob("[!]a-]", "b", GLOB_MATCHED);
    check_glob("[!]a-]", "]", GLOB_UNMATCHED);
    check_glob("[!]a-]", "a", GLOB_UNMATCHED);
    check_glob("[!]a-]", "-", GLOB_UNMATCHED);
    printf("\n");
    check_glob("[[?*\\]", "[", GLOB_MATCHED);
    check_glob("[[?*\\]", "?", GLOB_MATCHED);
    check_glob("[[?*\\]", "*", GLOB_MATCHED);
    check_glob("[[?*\\]", "\\", GLOB_MATCHED);
    check_glob("[[?*\\]", "a", GLOB_UNMATCHED);
    printf("\n");
    check_glob("\\*", "*", GLOB_MATCHED);
    printf("\n");
    check_glob("[Пп]ривет, [Мм]ир", "Привет, Мир", GLOB_MATCHED);
    return 0;
}
