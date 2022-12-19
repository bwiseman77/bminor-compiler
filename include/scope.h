#ifndef SCOPE_H
#define SCOPE_H

#include "symbol.h"
#include "hash_table.h"

/* structures */
struct symbol;

struct scope {
    struct hash_table *table;
    struct scope      *next;
    int                level;
    int                which;
};

/* functions */
struct scope  *scope_create();
struct scope  *scope_enter(struct scope *s);
struct scope  *scope_exit(struct scope *s);
int            scope_level(struct scope *s);
int            scope_bind(struct scope *s, const char *name, struct symbol *t);
struct symbol *scope_lookup(struct scope *s, const char *name);

#endif
