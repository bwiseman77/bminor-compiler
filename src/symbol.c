#include "symbol.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "expr.h"

/***********************************************************************
 * Create a symbol struct                                              *
 ***********************************************************************/
struct symbol * symbol_create(symbol_t kind, struct type *type, char *name) {
    struct symbol *s = malloc(sizeof(struct symbol));
    if (!s)
        return NULL;

    s->kind = kind;
    s->type = type_copy(type);
    s->name = name;
    s->which = 0;
    s->defined = 0;

    return s;
}

/***********************************************************************
 * Prints a symbol                                                     *
 ***********************************************************************/
void symbol_print(struct symbol *s) {
    switch (s->kind) {
        case SYMBOL_LOCAL:
            printf("%s resolves to local %d\n", s->name, s->which);
            break;
        case SYMBOL_GLOBAL:
            printf("%s resolves to global %s\n", s->name, s->name);
            break;
        case SYMBOL_PARAM:
            printf("%s resolves to param %d\n", s->name, s->which);
            break;
    }
}

/***********************************************************************
 * codegen a symbol                                                    *
 ***********************************************************************/
char *symbol_codegen(struct symbol *s) {
    char str[MAX_SYMBOL_SIZE];
    switch (s->kind) {
        case SYMBOL_GLOBAL:
            return s->name;
        case SYMBOL_PARAM:
            sprintf(str, "%d(%%rbp)", -8 * s->which);
            return strdup(str);
        case SYMBOL_LOCAL:
            sprintf(str, "%d(%%rbp)", -8 * s->which);
            return strdup(str);
        default:
            return 0;
    }
}
