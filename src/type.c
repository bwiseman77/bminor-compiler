#include "type.h"
#include "expr.h"
#include "error.h"
#include <stdlib.h>
#include <stdio.h>

/************************************************************************
 * creates an type struct and returns it                                *
 ***********************************************************************/
struct type * type_create(type_t kind, struct type *subtype, struct param_list *params) {
    struct type *t = malloc(sizeof(struct type));
    if(!t)
        return NULL;

    t->kind = kind;
    t->subtype = subtype;
    t->params = params;
    t->expr = 0;

    return t;
}


/*************************************************`*********************
 * prints a type by type_t                                              *
 ***********************************************************************/
void type_print(struct type *t) {
    switch (t->kind) {
        case TYPE_VOID:
            printf("void");
            break;
        case TYPE_BOOLEAN:
            printf("boolean");
            break;
        case TYPE_CHARACTER:
            printf("char");
            break;
        case TYPE_INTEGER:
            printf("integer");
            break;
        case TYPE_ARRAY:
            printf("array [");
            expr_print(t->expr);
            printf("] ");
            type_print(t->subtype);
            break;
        case TYPE_FUNCTION:
            printf("function ");
            type_print(t->subtype);
            printf(" (");
            param_list_print(t->params);
            printf(")");
            break;
        case TYPE_STRING:
            printf("string");
            break;
        case TYPE_AUTO:
            printf("auto");
            break;
        case TYPE_ARRAY_LIT:
            printf("array_lit");
        default:
            break;
    }  
}

/***********************************************************************
 * check if expr values of types are equal                             *
 ***********************************************************************/
int type_expr_value_equals(struct expr *a, struct expr *b) {
    if (!a && !b) return 1;
    if (!a || !b) return 0;

    return a->literal_value == b->literal_value;
}

/***********************************************************************
 * check if two types are equal                                        *
 ***********************************************************************/
int type_equals(struct type *a, struct type *b) {
    if (!a && !b) return 1;
    if (!a || !b) return 0;

    if(a->kind == b->kind
            && type_equals(a->subtype, b->subtype)
            && (param_equals(a->params, b->params) == 0)
            && type_expr_value_equals(a->expr, b->expr)) { 
        return 1;
    } else {
        return 0;
    }
}

/***********************************************************************
 * copy a type                                                         *
 ***********************************************************************/
struct type * type_copy(struct type *t) {
    if(!t) return 0;

    // copy subtype and if exists, copy expr
    struct type *c = type_copy(t->subtype);
    if(c) c->expr = expr_copy(t->subtype->expr);

    // copy base type, and if exists, copy expr
    struct type *l = type_create(t->kind, c, param_copy(t->params));
    if (l) l->expr = expr_copy(t->expr);
 
    return l;
}

/************************************************************************
 * converts a type of array literal to a proper array type              *
 * ie: {1, 2, 3, 4, 5} becomes array [5] integer                        *
 ***********************************************************************/
struct type * type_literal_convert(struct expr *e) {

    if(!e) return 0;
    // if not an array literal, means its a value inside so its just an expression so return its type
    // used as a base case
    if(e->kind != EXPR_ARRAY_LIT)
        return expr_typecheck(e);

    // compute the literal value and make sure each element matches
    int c = 0;
    struct type *first;
    for (struct expr *curr = e->mid; curr; curr=curr->next) {
        // use first as reference
        if(c == 0) {
            first = expr_typecheck(e->mid);
        } else {
            if(!type_equals(first, expr_typecheck(curr)))
                error_print(ERR_EXPR_LIT_MIS, e, 0);
        }

        c++;
    }

    // turn into an array
    struct type *t = type_create(TYPE_ARRAY, type_literal_convert(e->mid), 0);
    t->expr = expr_create_integer_literal(c);
    
    return t; 
}
