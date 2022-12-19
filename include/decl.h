#ifndef DECL_H
#define DECL_H

#include "scope.h"
#include "type.h"
#include "stmt.h"
#include "expr.h"
#include <stdio.h>

/* structures */
struct decl {
	char *name;
	struct type *type;
	struct expr *value;
	struct stmt *code;
	struct symbol *symbol;
	struct decl *next;

    int num_params;
    int num_locals;
    int num_total;
    int found_return;
};

/* functions */
/* constructors */
struct decl * decl_create( char *name, struct type *type, struct expr *value, struct stmt *code, struct decl *next );

/* print functions */
void decl_print( struct decl *d, int indent );
void indent_print(int indent);

/* resolving functions */
int decl_resolve(struct scope *s, struct decl *d);

/* typechecking functions */
void decl_typecheck( struct decl *d );

/* codegen functions */
void decl_codegen( struct decl *d, FILE *out );

#endif
