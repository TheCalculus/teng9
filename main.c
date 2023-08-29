#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "templator.h"
#include "api.h"

void parseTemplate(const char* template, Templite* templite) {
    size_t length = strlen(template);
    size_t curr   = 0;

    templite->elementCount = 0;
    templite->elements = NULL;

    while (curr < length) {
        const char* start = strstr(template + curr, "{{");
        if (start == NULL) break;

        const char* end = strstr(start, "}}");
        if (end == NULL) break;

        templite->elementCount++;
        templite->elements = (TemplateElement*)realloc(templite->elements, sizeof(TemplateElement) * templite->elementCount);

        templite->elements[templite->elementCount - 1].start = start - template;
        templite->elements[templite->elementCount - 1].end   = end + 2 - template;

        curr = end + 2 - template;
    }
}

void renderTemplate(const char* template, const Templite* templite, teng9_data data[], size_t amount) {
    size_t last = 0;

    for (size_t i = 0; i < templite->elementCount; i++) {
        printf("%.*s", (int)(templite->elements[i].start - last), template + last);

        size_t length = templite->elements[i].end - templite->elements[i].start - 4; // to exclude {{ and }} at the start and end
        char*  name   = (char*)malloc(length + 1);

        strncpy(name, template + templite->elements[i].start + 2, length);           // to exclude {{ at the start
        name[length] = '\0';

        printf("[%s]", data[i].value);

        free(name);
        last = templite->elements[i].end;
    }

    printf("%s\n", template + last);
}

static inline char*
executeElement(const char* element, teng9_expr parsemethod) {
switch (parsemethod) {
    char*       ws_javascript_queue[] = { }; // webserver javascript queue, obviously won't be here in the final version
    const char* observer =
        "    (r) => {                                                      \n"
        "        // a part of the injected javascript api                  \n"
        "        WS_ELEMENT_SET(document.getElementById(\"element\"),      \n"
        "                       r);                                        \n"
        "    }                                                             \n";

    case JS_EXPR_BROWSER:
        /* queue this code into js that has to be injected
           on execution of this javascript,
           take the result / return value and...
           */
        WS_JAVASCRIPT_ADD(element, observer);
        break;

    case C_EXPR_INTERP:
        /* queue this code into the c interpreter (you read that right) */
        BRUHCC_INTERPRETER(element);
        break;

    case C_EXPR_MACHINE:
        /* queue this code into the c compiler for execution */
        BRUHCC_COMPILER(element);
        break;
    
    case SIMPLE_STRING:
        /* this isn't code, return it */
        return (char*)element;
};

return "hello world";
}

teng9_data TENG9_CREATE_DATA(const char* value, teng9_expr parsemethod) {
    return
    (teng9_data) {
        .value       = (char*)value,
        .parsemethod = parsemethod,
    };
}

int main() {
    teng9_data name     = { .value = "John Doe",  .parsemethod = SIMPLE_STRING };
    teng9_data location = { .value = "Australia", .parsemethod = SIMPLE_STRING };

    const char* templateStr = "Hello {{name}}, welcome to {{location}}!";
    teng9_data  data[2]     = { name, location };

    Templite templite;
   
    parseTemplate (templateStr, &templite);
    renderTemplate(templateStr, &templite, data, 2);

    free(templite.elements);

    return 0;
}

