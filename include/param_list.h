
#ifndef PARAM_LIST_H
#define PARAM_LIST_H

#include "type.h"
#include "scope.h"
#include <stdio.h>

/* structures */
struct expr;
struct scope;

struct param_list {
	char *name;
	struct type *type;
	struct symbol *symbol;
	struct param_list *next;
    struct expr *error;
};

/* functions */
/* constructors */
struct param_list * param_list_create( char *name, struct type *type, struct param_list *next );

/* print functions */
void param_list_print( struct param_list *a );

/* resolve functions */
void param_resolve( struct scope *s, struct param_list *p );

/* typechecking functions */
int param_equals( struct param_list *a, struct param_list *b);
int param_typecheck ( struct param_list *a );
struct param_list * param_copy( struct param_list *p );

#endif
