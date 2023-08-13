#ifndef GLOB_H_
#define GLOB_H_

typedef enum {
    GLOB_UNMATCHED = 0,
    GLOB_MATCHED,
    GLOB_SYNTAX_ERROR,
} Glob_Result;

const char *glob_result_display(Glob_Result result);
Glob_Result glob(const uint32_t *pattern, const uint32_t *text);

#endif // GLOB_H_

#ifdef GLOB_IMPLEMENTATION

const char *glob_result_display(Glob_Result result)
{
    switch (result) {
    case GLOB_UNMATCHED:
        return "GLOB_UNMATCHED";
    case GLOB_MATCHED:
        return "GLOB_MATCHED";
    case GLOB_SYNTAX_ERROR:
        return "GLOB_SYNTAX_ERROR";
    default:
        assert(0 && "unreachable");
    }
}

Glob_Result glob(const uint32_t *pattern, const uint32_t *text)
{
    while (*pattern != '\0' && *text != '\0') {
        switch (*pattern) {
        case '?': {
            pattern += 1;
            text    += 1;
        }
        break;

        case '*': {
            Glob_Result result = glob(pattern + 1, text);
            if (result) return result;
            text += 1;
        }
        break;

        case '[': {
            bool matched = false;
            bool negate = false;

            pattern += 1; // skipping [
            if (*pattern == '\0') return GLOB_SYNTAX_ERROR; // unclosed [

            if (*pattern == '!') {
                negate = true;
                pattern += 1;
                if (*pattern == '\0') return GLOB_SYNTAX_ERROR; // unclosed [
            }

            uint32_t prev = *pattern;
            matched |= prev == *text;
            pattern += 1;

            while (*pattern != ']' && *pattern != '\0') {
                switch (*pattern) {
                case '-': {
                    pattern += 1;
                    switch (*pattern) {
                    case ']':
                        matched |= '-' == *text;
                        break;
                    case '\0':
                        return GLOB_SYNTAX_ERROR; // unclosed [
                    default: {
                        matched |= prev <= *text && *text <= *pattern;
                        prev = *pattern;
                        pattern += 1;
                    }
                    }
                }
                break;
                default: {
                    prev = *pattern;
                    matched |= prev == *text;
                    pattern += 1;
                }
                }
            }

            if (*pattern != ']') return GLOB_SYNTAX_ERROR; // unclosed [
            if (negate) matched = !matched;
            if (!matched) return GLOB_UNMATCHED;

            pattern += 1;
            text += 1;
        }
        break;

        case '\\':
            pattern += 1;
            if (*pattern == '\0') return GLOB_SYNTAX_ERROR; // unfinished escape
        // fallthrough
        default: {
            if (*pattern == *text) {
                pattern += 1;
                text    += 1;
            } else {
                return GLOB_UNMATCHED;
            }
        }
        }
    }

    if (*text == '\0') {
        while (*pattern == '*') pattern += 1;
        return *pattern == '\0';
    }

    return GLOB_UNMATCHED;
}

#endif // GLOB_IMPLEMENTATION
