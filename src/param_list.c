#include "param_list.h"
#include "symbol.h"
#include "scope.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************************************************************
 * creates an param_list struct and returns it                          *
 ***********************************************************************/
struct param_list * param_list_create(char *name, struct type *type, struct param_list *next) {
    struct param_list *p = malloc(sizeof(struct param_list));
    if(!p) 
        return NULL;

    p->name = strdup(name);
    p->type = type;
    p->next = next;
    return p;
}

/************************************************************************
 * prints a praram_list                                                 *
 ***********************************************************************/
void param_list_print(struct param_list *p) {
    if(!p) return;
    printf("%s: ", p->name);
    type_print(p->type);
    if(p->next != NULL)
        printf(", ");
    param_list_print(p->next);
}

/************************************************************************
 * resolves a praram_list                                               *
 ***********************************************************************/
void param_resolve(struct scope *s, struct param_list *p) {
    for(struct param_list *t = p; t; t = t->next) {
        struct symbol *n = symbol_create(SYMBOL_PARAM, t->type, t->name);
        scope_bind(s, t->name, n);
        n->which = s->which;
    }
}

/************************************************************************
 * check if two praram_lists are equal                                  *
 ***********************************************************************/
int param_equals(struct param_list *a, struct param_list *b) {
    if (!a && !b) return 0;
    if (!a || !b) return 1;

    int status = 0;
    if(!type_equals(a->type, b->type)) {
        status++;
    }

    if(param_equals(a->next, b->next)) {
        status++;
    }
    return status;
}

/************************************************************************
 * typechecks a praram_list                                             *
 ***********************************************************************/
int param_typecheck(struct param_list *a) {
    struct param_list *t = a;
    while(a) {
        if(a->type->kind == TYPE_AUTO)
            return 1;
        a = a->next;
    } 
    return 0;
}

/************************************************************************
 * copies a praram_list                                                 *
 ***********************************************************************/
struct param_list * param_copy(struct param_list *p) {
    if(!p) return 0;

    return  param_list_create(p->name, p->type, param_copy(p->next));
}
