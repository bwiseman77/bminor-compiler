#ifndef TYPE_H
#define TYPE_H

#include "param_list.h"

/* structures */

typedef enum {
	TYPE_VOID,
	TYPE_BOOLEAN,
	TYPE_CHARACTER,
	TYPE_INTEGER,
	TYPE_STRING,
	TYPE_ARRAY,
    TYPE_ARRAY_LIT,
	TYPE_FUNCTION,
    TYPE_AUTO
} type_t;

struct type {
	type_t kind;
	struct param_list *params;
	struct type *subtype;
    struct expr *expr;
};

/* functions */
/* constructor */
struct type * type_create( type_t kind, struct type *subtype, struct param_list *params );

/* print functions */
void  type_print( struct type *t );

/* typechecking functions */
int           type_equals( struct type *a, struct type *b );
struct type * type_copy( struct type *t );
struct type * type_literal_convert( struct expr *e );
int           type_delete( struct type *t );
int           type_arraylit_check( struct expr *e );

#endif
