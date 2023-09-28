#ifndef TEMPLATOR_H
#define TEMPLATOR_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    size_t start;
    size_t end;
} TemplateElement;

typedef struct {
    TemplateElement* elements;
    size_t           elementCount;
} Templite;

typedef enum {
    JS_EXPR_BROWSER,
    C_EXPR_INTERP,
    C_EXPR_MACHINE,
    SIMPLE_STRING,
} teng9_expr;

typedef struct {
    char*         value;
    teng9_expr    parsemethod;
} teng9_data;

void parseTemplate (const char* template_, Templite* templite);
void renderTemplate(const char* template_, const Templite* templite, teng9_data data[], size_t amount);

static inline char* executeElement   (const char* element, teng9_expr parsemethod);
teng9_data          TENG9_CREATE_DATA(const char* value, teng9_expr parsemethod);

#endif
