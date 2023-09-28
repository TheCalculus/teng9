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
void nextValidCharacter() {
    // it aint pretty
    // but it aint broke
    while ((lexer->active =
                getc(lexer->buffer)) != EOF &&
               (isspace(lexer->active)      ||
               !isalnum(lexer->active)      ||
               !isprint(lexer->active)
               )
          );
}

static inline
void getTokenType(Token* token, int* depth) {
    (*depth)++;

    switch (lexer->active) {
    case '{':
        token->type = SIMPLE_DELIMITER_LEFT;

        nextValidCharacter();
        getTokenType(token, depth);

        ungetc(lexer->active, lexer->buffer);
        
        return;

    case '}':
        token->type = SIMPLE_DELIMITER_RIGHT;

        switch (ungetc(lexer->active, lexer->buffer)) {
        case '%': token->type = EXPRESSION_DELIMITER_RIGHT;  return;
        case '$': token->type = INTERPRETER_DELIMITER_RIGHT; return;
        case '#': token->type = COMPILATION_DELIMITER_RIGHT; return;
        }

        nextValidCharacter();
        nextValidCharacter();
        getTokenType(token, depth);

        ungetc(lexer->active, lexer->buffer);

        return;
  
    case '%':
        token->type = EXPRESSION_DELIMITER_LEFT;

        nextValidCharacter();
        getTokenType(token, depth);

        return;

    case '$':
        token->type = INTERPRETER_DELIMITER_LEFT;

        nextValidCharacter();
        getTokenType(token, depth);

        return;

    case '#':
        token->type = COMPILATION_DELIMITER_LEFT;

        nextValidCharacter();
        getTokenType(token, depth);
        
        return;

    default:
        if (*depth == 1) scanLiterals(token);
    }
}

void tokenize() {
    if (lexer == NULL) perror("NULL lexer");

    Token token;
    int   depth = 0;

    while ((lexer->active = getc(lexer->buffer)) != EOF) {
        if (isspace(lexer->active) || !isprint(lexer->active))
            continue;

        depth       = 0;
        token.size  = 1;
        
        token.value = (char*)malloc(sizeof(char) * token.size);
        strcpy(token.value, &lexer->active);

        getTokenType(&token, &depth);
        printf("token: %s (%d)\n", token.value, token.type);

nocharalloc:
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
