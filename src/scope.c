#include "scope.h"
#include <stdlib.h>

/* create a new scope struct */
struct scope * scope_create() {
    struct scope *s = malloc(sizeof(struct scope));
    s->table = hash_table_create(0,0);
    s->level = 0;
    s->next  = 0;
    s->which = 0;
    return s;
}

/* delete a scope struct */
void scope_delete(struct scope *s) {
    hash_table_delete(s->table);
    free(s);
}

/* adds new scope struct in front of given scope struct *
 * top of stack -> n->next -> s                         */
struct scope * scope_enter(struct scope *s) {
    struct scope *n = scope_create();
    n->next = s;
    n->level = s->level+1;
    return n;
}

/* pop a scope */
struct scope * scope_exit(struct scope *s) {
    struct scope *n = s->next;
    scope_delete(s);
    return n;
}

/* get a scope level (not really needed but for completeness */
int scope_level(struct scope *s) {
    return s->level;
}

/* enter a name into the hash table */
int scope_bind(struct scope *s, const char *name, struct symbol *t) {
    if(hash_table_insert(s->table, name, t) == 1) {
        s->which++;
        return 1;
    }
    struct symbol *v = hash_table_lookup(s->table, name);
    if(t->type->kind != TYPE_FUNCTION) {
        printf("resolve error: %s is already defined!\n", t->name);
        return 0;
    }
    if(t->type->kind == TYPE_FUNCTION && !type_equals(t->type, v->type)) {
        printf("resolve error: types of %s do not match!\n", t->name);
        return 0;
    }
    return 1;
}

/* check topmost scope first, then check bottom ones for a given name */
struct symbol * scope_lookup(struct scope *s, const char *name) {
    struct scope *t = s;
    while(t) {
        struct symbol *y = hash_table_lookup(t->table, name);
        if(y)
            return y;
        t = t->next;
    }
    return 0;
}
