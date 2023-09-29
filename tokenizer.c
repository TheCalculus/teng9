// courtesy of the original chasm repository
// no code goes waste... expect templator_parlex or templator_parser. they suck.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "tokenizer.h"

Lexer* lexer = NULL;

void initLexer(const char* file) {
    lexer = (Lexer*)malloc(sizeof(Lexer));

    lexer->token_capacity = 10;
    lexer->token_volume   = 0;

    lexer->tokens = (Token*)malloc(sizeof(Token) * lexer->token_capacity);

    lexer->buffer = fopen(file, "r");
}

static inline
void scanLiterals(Token* token) {
    if (isalpha(lexer->active)) {
        char* literal = (char*)malloc(sizeof(char) * 10);
        int      size = 0;

        do {
            literal[size++] = lexer->active;
            lexer->active = getc(lexer->buffer);
        } while (isalnum(lexer->active) && size < 10);

        literal[++size] = '\0';

        token->type  = LITERAL;

        if (token->value == NULL) token->value = (char*)malloc(sizeof(char) * size);
        else                      token->value = (char*)realloc(token->value, sizeof(char) * size);

        strcpy(token->value, literal);
        
        if (!isspace(lexer->active))
            ungetc(lexer->active, lexer->buffer);

        free(literal);
        token->size  = size;
    }
}

static inline
bool isvalid() {
    return lexer->active != EOF
           && !isspace(lexer->active)
           && isprint(lexer->active);
}

static inline
void nextValidCharacter() {
    while (!isvalid())
        getc(lexer->buffer);
}

static inline
void getTokenType(Token* token) {
    switch (*token->value) {
    case '{':
        nextValidCharacter();

        switch (lexer->active) {
            case '{':
                break;
            case '%': 
                break;
            case '$': 
                break;
            case '#': 
                break;
            default:
                scanLiterals(token);
        }

        break;

    case '}':
        break;

    default:
        scanLiterals(token);
    }
}

void tokenize() {
    if (lexer == NULL) perror("NULL lexer");

    Token token;

    while (true) {
        if (lexer->active == EOF) break;
        if (!isvalid()) goto next;

        token.size  = 1;
        token.value = (char*)malloc(sizeof(char));

        strcpy(token.value, &lexer->active);

        getTokenType(&token);
        printf("token: %s (%d)\n", token.value, token.type);

        if(lexer->token_volume >= lexer->token_capacity) {
            lexer->token_capacity += 10;
            lexer->tokens = (Token*)realloc(lexer->tokens, lexer->token_capacity * sizeof(Token));
            printf("Token* resized to %zu\n", lexer->token_capacity);
        }

        ungetc(lexer->active, lexer->buffer);
        memcpy(&lexer->tokens[lexer->token_volume], &token, sizeof(token));

        lexer->token_volume++;
next:
        printf("going to next char");
        lexer->active = getc(lexer->buffer);
    }

    lexer->tokens[lexer->token_volume++] = (Token) { END_OF_FILE, "", 1 };
}

void free_resources() {
    fclose(lexer->buffer);
    free(lexer->tokens); 
    free(lexer);
}
