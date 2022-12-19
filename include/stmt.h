#ifndef STMT_H
#define STMT_H

#include "decl.h"
#include "scratch.h"

/* structures */
typedef enum {
	STMT_DECL,
	STMT_EXPR,
	STMT_IF_ELSE,
	STMT_FOR,
	STMT_PRINT,
	STMT_RETURN,
	STMT_BLOCK,
    STMT_WHILE
} stmt_t;

struct stmt {
	stmt_t kind;
	struct decl *decl;
	struct expr *init_expr;
	struct expr *expr;
	struct expr *next_expr;
	struct stmt *body;
	struct stmt *else_body;
	struct stmt *next;
    int else_if;            // if part of a else
    int fi;                 // if part of if
    int no_indent;          // mark if shouldnt indent
    char *name;             // name of function of return statement

    int is_func;
};

/* functions */
/* constructor */
struct stmt * stmt_create( stmt_t kind, struct decl *decl, struct expr *init_expr, struct expr *expr, struct expr *next_expr, struct stmt *body, struct stmt *else_body, struct stmt *next );

/* print functions */
void stmt_print( struct stmt *s, int indent );
void stmt_decl_print( struct stmt *s, int indent );
void stmt_expr_print( struct stmt *s, int indent );
void stmt_if_else_print( struct stmt *s, int indent );
void stmt_for_print( struct stmt *s, int indent );
void stmt_block_print( struct stmt *s, int indent );
void stmt_return_print( struct stmt *s, int indent );
void stmt_print_print( struct stmt *s, int indent );
void stmt_while_print( struct stmt *s, int indent );

/* resolve functions */
int stmt_resolve( struct scope *c, struct stmt *s);

/* typechecking functions */
struct type * stmt_typecheck( struct stmt *s, struct decl *r );

/* codegen functions */
void stmt_codegen( struct stmt *s, FILE *out );

#endif
