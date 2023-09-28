// courtesy of the original chasm repository
// no code goes waste... expect templator_parlex or templator_parser. they suck.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "tokenizer.h"

Lexer* lexer = NULL;

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

        token->type = LITERAL;
        token->value = (char*)realloc(token->value, sizeof(char) * size);

        strcpy(token->value, literal);
        
        if (!isspace(lexer->active))
            ungetc(lexer->active, lexer->buffer);

        free(literal);
        token->size  = size;
    }
}

void tokenize() {
    while ((lexer->active = getc(lexer->buffer)) != EOF) {
        if (isspace(lexer->active) || !isprint(lexer->active))
            continue;

        Token token;
        token.size  = 1;
        token.value = (char*)malloc(sizeof(char) * token.size);
        strcpy(token.value, &lexer->active);

        switch (lexer->active) {
            case '{': break;
            default: scanLiterals(&token);
        }

        if(lexer->token_volume >= lexer->token_capacity) {
            /* resize token sequence by an arbitrary value to allow for more tokens */
            lexer->token_capacity += 50;  // todo: figure out an efficient way to increase size
            lexer->tokens = (Token*)realloc(lexer->tokens, lexer->token_capacity * sizeof(Token));
            printf("Token* resized to %zu\n", lexer->token_capacity);
        }

        ungetc(lexer->active, lexer->buffer); /* the holy function */
        memcpy(&lexer->tokens[lexer->token_volume], &token, sizeof(token));

        lexer->active = getc(lexer->buffer);
        lexer->token_volume++;
    }

    lexer->tokens[lexer->token_volume++] = (Token) { END_OF_FILE, "", 1 };
}

void free_resources() {
    fclose(lexer->buffer);
    free(lexer->tokens); 
    free(lexer);
}
