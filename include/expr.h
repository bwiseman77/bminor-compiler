#ifndef EXPR_H
#define EXPR_H

#include "symbol.h"
#include "scope.h"
#include "decl.h"
#include "scratch.h"

/* structures */

struct decl;

typedef enum {
	EXPR_ASSIGN = 0,
    EXPR_TERN,
    
    EXPR_OR = 10,

    EXPR_AND = 20,
    
    EXPR_LT = 30,
    EXPR_LTE,
    EXPR_GT,
    EXPR_GTE,
    EXPR_EQ,
    EXPR_NEQ,
    
    EXPR_ADD = 40,
	EXPR_SUB,
    
    EXPR_MUL = 50,
	EXPR_DIV,
    EXPR_MOD,
    
    EXPR_EX = 60,
    
    EXPR_NOT = 70,
    EXPR_NEG,
    
    EXPR_INC = 80,
    EXPR_DEC,

    EXPR_FUNC = 90,
    EXPR_ARRAY,
    EXPR_GROUP,


    EXPR_CHAR = 100,
    EXPR_IDENT,
    EXPR_INT,
    EXPR_STR,
    EXPR_BOOL,
    EXPR_ARRAY_LIT
} expr_t;

struct expr {
	expr_t kind;                    // kind of expr
	struct expr *left;              // left node
	struct expr *right;             // right node
    struct expr *mid;               // middle node, or node of function call/array subscript

    int group;                      // notes if it is a (), 1 means yes, 0 means no
    struct expr *next;              // next expr for expr lists
	const char *name;               // name of expr (idents)
	int literal_value;              // literal value for ints and bools
	const char * string_literal;    // string lit for str and char
	struct symbol *symbol;          // symbol for scope
    struct type *error;             // for debugging
    int reg;                        // value of register
    int bad_array_mid;              // for debugging
    int label;                      // label value
    int loop_expr;                  // if an expr is part of a loop
    int func_value;                 // if return value is needed from function
};

/* functions */

/* constructor */
struct expr * expr_create( expr_t kind, struct expr *left, struct expr *right );

/* literal constuctors */
struct expr * expr_create_name( const char *n );
struct expr * expr_create_integer_literal( int c );
struct expr * expr_create_boolean_literal( int c );
struct expr * expr_create_char_literal( const char * c );
struct expr * expr_create_string_literal( const char *str );
struct expr * expr_create_array_subscript( const char *n, expr_t kind, struct expr *mid );
struct expr * expr_create_func_call( const char *n, expr_t kind, struct expr *mid );
struct expr * expr_create_tern( struct expr *left, struct expr *mid, struct expr *right );
struct expr * expr_create_array_literal( struct expr *a );

/* print functions */
void expr_print( struct expr *e );
void expr_list_print( struct expr *e );
void print_expr_t( struct expr *e );
void expr_print_array_lit( struct expr *e );
void expr_array_print( struct expr *e );

/* resolve functions */
int expr_resolve( struct scope *s, struct expr *e );

/* typechecking functions */
struct type * expr_typecheck( struct expr *e );
void expr_array_lit_check( struct expr *e, struct decl *d, struct type *t );
struct expr * expr_copy( struct expr *e );

/* codegen functions */
void expr_codegen( struct expr *, FILE *);

#endif
