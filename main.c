#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t start;
    size_t end;
} TemplateElement;

typedef struct {
    TemplateElement* elements;
    size_t elementCount;
} Templite;

typedef struct { float data; } TENG9_FLOAT;
typedef struct { char  data; } TENG9_CHAR;
typedef struct { int   data; } TENG9_INT;
typedef struct { char* data; } TENG9_STRING;

typedef enum {
    CHARACTER,
    FLOATING,
    INTEGER,
    STRING,
} TENG9_FLAG;

typedef union {
    TENG9_CHAR   character;
    TENG9_FLOAT  floating;
    TENG9_INT    integer;
    TENG9_STRING string;
} TENG9_DATATYPES;

typedef struct {
    TENG9_FLAG      type;
    TENG9_DATATYPES data;
} TENG9_DATA;

TENG9_DATA* TENG9_CREATEDATA(TENG9_FLAG flag, void* dat) {
    TENG9_DATA* tmp = (TENG9_DATA*)malloc(sizeof(TENG9_DATA));
    tmp->type = flag;

    switch (flag) {
        case CHARACTER:  tmp->data.character = *((TENG9_CHAR*)dat);   break;
        case FLOATING:   tmp->data.floating  = *((TENG9_FLOAT*)dat);  break;
        case INTEGER:    tmp->data.integer   = *((TENG9_INT*)dat);    break;
        case STRING:     tmp->data.string    = *((TENG9_STRING*)dat); break;
    }

    return tmp;
}

typedef const char* (*DataToStringFunc)(TENG9_DATATYPES);

const char* CharToString(TENG9_DATATYPES data) {
    static char str[2];
    str[0] = data.character.data;
    str[1] = '\0';
    return str;
}

const char* FloatToString(TENG9_DATATYPES data) {
    static char str[32];
    snprintf(str, sizeof(str), "%f", data.floating.data);
    return str;
}

const char* IntToString(TENG9_DATATYPES data) {
    static char str[32];
    snprintf(str, sizeof(str), "%d", data.integer.data);
    return str;
}

const char* StringToString(TENG9_DATATYPES data) {
    return data.string.data;
}

DataToStringFunc ToStringFuncs[] = {
    CharToString,
    FloatToString,
    IntToString,
    StringToString
};

const char* ToString(TENG9_DATA data) {
    return ToStringFuncs[data.type](data.data);
}

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

void renderTemplate(const char* template, const Templite* templite, TENG9_DATA data[], size_t amount) {
    size_t last = 0;

    for (size_t i = 0; i < templite->elementCount; i++) {
        printf("%.*s", (int)(templite->elements[i].start - last), template + last);

        size_t length = templite->elements[i].end - templite->elements[i].start - 4; // to exclude {{ and }} at the start and end
        char*  name   = (char*)malloc(length + 1);

        strncpy(name, template + templite->elements[i].start + 2, length);           // to exclude {{ at the start
        name[length] = '\0';

        printf("[%s]", ToString(data[i]));

        free(name);
        last = templite->elements[i].end;
    }

    printf("%s\n", template + last);
}

int main() {
    const char* templateStr = "Hello {{name}}, welcome to {{location}}!";

    TENG9_STRING name     = { "John Doe" };
    TENG9_STRING location = { "Australia" };

    TENG9_DATA data[2] = {
        *TENG9_CREATEDATA(STRING, &name),
        *TENG9_CREATEDATA(STRING, &location)
    };

    Templite templite;
   
    parseTemplate (templateStr, &templite);
    renderTemplate(templateStr, &templite, data, 2);

    free(templite.elements);

    return 0;
}

