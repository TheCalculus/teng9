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
    while (isspace(*active) || !isprint(*active)) {
        *active = getc(tok.buffer);
        if (*active == EOF) return EOF;
    }

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
            if (n == EOF) break;
            if (n != '{') continue;

            curr.type  = LEFT_DELIMITER;
            curr.value = strdup("{{");

            break;
        case '}':
            if (n == EOF) break;
            if (vnext() != '}') continue;

            curr.type  = RIGHT_DELIMITER;
            curr.value = strdup("}}");

            break;
        case '#': break;
        case '/': break;
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

    if (buffer == NULL) {
        perror("Error opening file");
        return 1;
    }

    tokenize();
    cleanup();

    fclose(buffer);

    return 0;
}
