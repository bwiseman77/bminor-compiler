#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"
#include "scratch.h"

/* structures */

typedef enum {
	SYMBOL_LOCAL,
	SYMBOL_PARAM,
	SYMBOL_GLOBAL
} symbol_t;

struct symbol {
	symbol_t kind;
	struct type *type;
	char *name;
	int which;
    int defined;
};

/* functions */
/* constructor */
struct symbol * symbol_create( symbol_t kind, struct type *type, char *name );

/* print function */
void symbol_print( struct symbol *s );

/* codegen functions */
char *symbol_codegen( struct symbol *s );

#endif
