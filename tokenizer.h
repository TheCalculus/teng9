#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    SIMPLE_DELIMITER_LEFT,       // {{
    SIMPLE_DELIMITER_RIGHT,      //  }}
    EXPRESSION_DELIMITER_LEFT,   // {%
    EXPRESSION_DELIMITER_RIGHT,  //  %}
    INTERPRETER_DELIMITER_LEFT,  // {ci
    INTERPRETER_DELIMITER_RIGHT, //  *}
    COMPILATION_DELIMITER_LEFT,  // {c
    COMPILATION_DELIMITER_RIGHT, //  *}
    LITERAL,
    END_OF_FILE,
} TokenType;

typedef struct {
    TokenType type;
    char*     value;
    size_t    size;
    uint8_t   row, col;
} Token;

typedef struct {
    char    active;
    uint8_t row, col;
    FILE*   buffer;
    Token*  tokens;
    size_t  token_capacity;
    size_t  token_volume;
} Lexer;

extern Lexer* lexer;

static inline
void scanLiterals(Token* token);
void tokenize();
void free_resources();

#endif
