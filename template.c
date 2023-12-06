#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    LEFT_DELIMITER,
    RIGHT_DELIMITER,
    BEGIN_EXPRESSION,
    END_EXPRESSION,
    LABELLED_EXPRESSION,
} type;

typedef struct {
    char*   value;
    size_t  size;
    type    type;
} token;

typedef struct {
    FILE*   buffer;
    token*  tokens;
    size_t  ind, cap;
    uint8_t row, col;
} tokenizer;

tokenizer tok;

char next(char* active) {
    do {
        *active = getc(tok.buffer);
        if (*active == EOF) return EOF;
    } while (isspace(*active) || !isprint(*active));

    return *active;
}

char vnext() {
    return getc(tok.buffer);
}

void tokenize() {
    char active;

    while ((active = next(&active)) != EOF) {
        token curr;

        char n = vnext();
        switch (active) {
            case '{':
                if (n != '{') continue;

                curr.type  = LEFT_DELIMITER;
                curr.value = strdup("{{");
                curr.size  = 2;

                break;
            case '}':
                if (vnext() != '}') continue;

                curr.type  = RIGHT_DELIMITER;
                curr.value = strdup("}}");
                curr.size  = 2;

                break;
            case '#':
                curr.type  = BEGIN_EXPRESSION;
                curr.value = strdup("#");
                curr.size  = 1;

                break;
            case '/':
                curr.type  = END_EXPRESSION;
                curr.value = strdup("/");
                curr.size  = 1;

                break;
            default:
                {
                    int     i  = 0;
                    size_t sz  = 10;

                    curr.value = (char*)malloc(sz + 1);

                    do {
                        curr.value[i++] = n;

                        if (i == sz - 1) {
                            sz += 10;
                            curr.value = (char*)realloc(curr.value, sz + 1);
                        }
                        n = vnext();
                    } while (isalnum(n));

                    curr.value[i] = '\0';

                    printf("%s\n", curr.value);
                }
        }

        tok.tokens[tok.ind++] = curr;
        if (tok.ind == tok.cap) {
            tok.cap += 10;
            tok.tokens = (token*)realloc(tok.tokens, tok.cap * sizeof(token));
        }
    }
}

void cleanup() {
    for (size_t i = 0; i < tok.ind; i++) free(tok.tokens[i].value);
    free(tok.tokens);
}

int main() {
    FILE* buffer = fopen("nice_template.ch", "rb");
    tok.buffer   = buffer;
    tok.tokens   = (token*)malloc(10 * sizeof(token));

    if (buffer == NULL) {
        perror("error opening file");
        return 1;
    }

    tokenize();
//  cleanup();

    fclose(buffer);

    return 0;
}
