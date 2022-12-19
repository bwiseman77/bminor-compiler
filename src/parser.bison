%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stmt.h"
#include "decl.h"
#include "symbol.h"
#include "expr.h"
extern char *yytext;
extern int yylex();
extern int yyerror( char *str );

struct decl * parser_result;
%}
    /* TOKENS */
%token    TOKEN_ERROR
%token    TOKEN_BAD_STRING
%token    TOKEN_BAD_COMMENT
%token    TOKEN_BAD_CHAR
%token    TOKEN_BAD_IDENTIFIER

    /* whitespace */
%token    TOKEN_MULTI_LINE
%token    TOKEN_SINGLE_LINE
%token    TOKEN_WHITESPACE
    
    /* keywords */
%token    TOKEN_ARRAY
%token    TOKEN_AUTO
%token    TOKEN_BOOLEAN
%token    TOKEN_CHAR
%token    TOKEN_ELSE
%token    TOKEN_FALSE
%token    TOKEN_FOR
%token    TOKEN_FUNCTION
%token    TOKEN_IF
%token    TOKEN_INTEGER
%token    TOKEN_PRINT
%token    TOKEN_RETURN
%token    TOKEN_STRING
%token    TOKEN_TRUE
%token    TOKEN_VOID
%token    TOKEN_WHILE

    /* expressions */
%token    TOKEN_ADD
%token    TOKEN_SUB
%token    TOKEN_ASSIGN
%token    TOKEN_INCREMENT
%token    TOKEN_DECREMENT
%token    TOKEN_NOT
%token    TOKEN_EXPONENT
%token    TOKEN_MULTIPLICATION
%token    TOKEN_DIVISON
%token    TOKEN_MODULO
%token    TOKEN_LT
%token    TOKEN_LTE
%token    TOKEN_GT
%token    TOKEN_GTE
%token    TOKEN_EQ
%token    TOKEN_NOT_EQ
%token    TOKEN_AND
%token    TOKEN_OR
%token    TOKEN_TERNARY_PT_1

    /* literals */
%token    TOKEN_IDENT
%token    TOKEN_CHAR_LITERAL
%token    TOKEN_STRING_LITERAL
%token    TOKEN_INTEGER_LITERAL

    /* misc */
%token    TOKEN_COLON
%token    TOKEN_SEMICOLON
%token    TOKEN_COMMA
%token    TOKEN_SINGLE_QUOTE
%token    TOKEN_DOUBLE_QUOTE
%token    TOKEN_LEFT_BRACE
%token    TOKEN_RIGHT_BRACE
%token    TOKEN_LEFT_BRACKET
%token    TOKEN_RIGHT_BRACKET
%token    TOKEN_LEFT_PARENTH
%token    TOKEN_RIGHT_PARENTH

    /* UNIONS */
%union {
    struct decl *decl;
    struct expr *expr;
    struct stmt *stmt;
    struct symbol *symbol;
    struct type *type;
    struct param_list *param_list;
    struct expr_list *expr_list;
    char *name;
};

    /* TYPES */
%type <decl> prog bunch_o_decls decl
%type <stmt> bunch_o_stmts_e bunch_o_stmts stmt stmts if_stmt2 if_stmt 
%type <expr> expr expr1 expr2 expr3 expr4 expr5 expr6 expr7 expr8 expr9 atomic arg index_list expr_list_e expr_list
%type <name> ident
%type <type> type
%type <param_list> param_list param_list_e param 

%%
/*------------------------------------------+
| basic program                             |
+------------------------------------------*/
prog: bunch_o_decls
        { parser_result = $1; }
    ;

/*------------------------------------------+
| expressions with precedence (low to high) |
+------------------------------------------*/
expr: expr1 TOKEN_ASSIGN expr
       { $$ = expr_create(EXPR_ASSIGN, $1, $3); }
    | expr1 TOKEN_TERNARY_PT_1 expr TOKEN_COLON expr
       { $$ = expr_create_tern($1, $3, $5); }
    | expr1
       { $$ = $1; }
    ;

expr1: expr1 TOKEN_OR expr2
        { $$ = expr_create(EXPR_OR, $1, $3); }
     | expr2
        { $$ = $1; }
     ;

expr2: expr2 TOKEN_AND expr3
        { $$ = expr_create(EXPR_AND, $1, $3); }
     | expr3
        { $$ = $1; }
     ;

expr3: expr3 TOKEN_LT expr4
        { $$ = expr_create(EXPR_LT, $1, $3); }
     | expr3 TOKEN_LTE expr4
        { $$ = expr_create(EXPR_LTE, $1, $3); }
     | expr3 TOKEN_GT expr4
        { $$ = expr_create(EXPR_GT, $1, $3); }
     | expr3 TOKEN_GTE expr4
        { $$ = expr_create(EXPR_GTE, $1, $3); }
     | expr3 TOKEN_EQ expr4
        { $$ = expr_create(EXPR_EQ, $1, $3); }
     | expr3 TOKEN_NOT_EQ expr4
        { $$ = expr_create(EXPR_NEQ, $1, $3); }
     | expr4
        { $$ = $1; }
     ;

expr4: expr4 TOKEN_ADD expr5
        { $$ = expr_create(EXPR_ADD, $1, $3); }
     | expr4 TOKEN_SUB expr5
        { $$ = expr_create(EXPR_SUB, $1, $3); }
     | expr5
        { $$ = $1; }
     ;

expr5: expr5 TOKEN_MULTIPLICATION expr6
        { $$ = expr_create(EXPR_MUL, $1, $3); }
     | expr5 TOKEN_DIVISON expr6
        { $$ = expr_create(EXPR_DIV, $1, $3); }
     | expr5 TOKEN_MODULO expr6
        { $$ = expr_create(EXPR_MOD, $1, $3); }
     | expr6
        { $$ = $1; }
     ;

expr6: expr7 TOKEN_EXPONENT expr6
        { $$ = expr_create(EXPR_EX, $1, $3); }
     | expr7
        { $$ = $1; }
     ;

expr7: TOKEN_NOT expr7
        { $$ = expr_create(EXPR_NOT, 0, $2); }
     | TOKEN_SUB expr7
        { $$ = expr_create(EXPR_NEG, 0, $2); }
     | expr8
        { $$ = $1; }
     ;

expr8: expr8 TOKEN_INCREMENT
        { $$ = expr_create(EXPR_INC, $1, 0); }
     | expr8 TOKEN_DECREMENT
        { $$ = expr_create(EXPR_DEC, $1, 0); }
     | expr9
        { $$ = $1; }
     ;

expr9: TOKEN_LEFT_PARENTH expr TOKEN_RIGHT_PARENTH
        { $$ = $2; $$->group = 1; }
     | ident index_list
        { $$ = expr_create_array_subscript($1, EXPR_ARRAY, $2);  }
     | ident TOKEN_LEFT_PARENTH expr_list_e TOKEN_RIGHT_PARENTH
        { $$ = expr_create_func_call($1, EXPR_FUNC, $3); }
     | atomic
        { $$ = $1; }
     ;

/*-----------------------------------------+
| atomic values                            |
+-----------------------------------------*/
atomic: ident
        { $$ = expr_create_name($1); }
      | TOKEN_INTEGER_LITERAL
        { $$ = expr_create_integer_literal(atoi(yytext)); }
      | TOKEN_STRING_LITERAL
        { $$ = expr_create_string_literal(strdup(yytext)); }
      | TOKEN_CHAR_LITERAL
        { $$ = expr_create_char_literal(strdup(yytext)); }
      | TOKEN_FALSE
        { $$ = expr_create_boolean_literal(0); }
      | TOKEN_TRUE
        { $$ = expr_create_boolean_literal(1); }
      | TOKEN_LEFT_BRACE expr_list TOKEN_RIGHT_BRACE
        { $$ = expr_create_array_literal($2); }
      ;

/*-----------------------------------------+
| identifier                               |
+-----------------------------------------*/
ident: TOKEN_IDENT
        { $$ = strdup(yytext); }
     ;

/*-----------------------------------------+
| types                                    |
+-----------------------------------------*/
type: TOKEN_STRING
        { $$ = type_create(TYPE_STRING, 0, 0); }
    | TOKEN_INTEGER
        { $$ = type_create(TYPE_INTEGER, 0, 0); }
    | TOKEN_BOOLEAN
        { $$ = type_create(TYPE_BOOLEAN, 0, 0); }
    | TOKEN_CHAR
        { $$ = type_create(TYPE_CHARACTER, 0, 0); }
    | TOKEN_VOID
        { $$ = type_create(TYPE_VOID, 0, 0); }
    | TOKEN_AUTO
        { $$ = type_create(TYPE_AUTO, 0, 0); }
    | TOKEN_ARRAY TOKEN_LEFT_BRACKET arg TOKEN_RIGHT_BRACKET type
        { $$ = type_create(TYPE_ARRAY, $5, 0); $$->expr = $3; }
    | TOKEN_FUNCTION type TOKEN_LEFT_PARENTH param_list_e TOKEN_RIGHT_PARENTH
        { $$ = type_create(TYPE_FUNCTION, $2, $4); }
    ;

/*-----------------------------------------+
| list: func params, fors, indexs, exprs   |
+-----------------------------------------*/
param_list_e: param_list
                { $$ = $1; }
            |
                { $$ = NULL; }
            ;

param_list: param TOKEN_COMMA param_list
                { $1->next = $3; $$ = $1; }    
          | param
                { $$ = $1; }
          ;

param: ident TOKEN_COLON type
        { $$ = param_list_create($1, $3, 0); }
     ;

index_list: TOKEN_LEFT_BRACKET expr TOKEN_RIGHT_BRACKET index_list
            { $2->next = $4; $$ = $2; }
          | TOKEN_LEFT_BRACKET expr TOKEN_RIGHT_BRACKET 
            { $$ = $2; }
          ;

expr_list_e: expr_list
                { $$ = $1; }
            |
                { $$ = NULL; }
            ;

expr_list: expr TOKEN_COMMA expr_list
            { $1->next = $3; $$ = $1; }
         | expr
            { $$ = $1; }
         ;

/*-----------------------------------------+
| args: for loop                           |
+-----------------------------------------*/
arg: expr
      { $$ = $1; }
   | 
      { $$ = NULL; }
   ;

/*-----------------------------------------+
| declarations                             |
+-----------------------------------------*/
decl: ident TOKEN_COLON type TOKEN_ASSIGN expr TOKEN_SEMICOLON
        { $$ = decl_create($1, $3, $5, 0, 0); }
    | ident TOKEN_COLON type TOKEN_ASSIGN TOKEN_LEFT_BRACE bunch_o_stmts_e TOKEN_RIGHT_BRACE
        { $$ = decl_create($1, $3, 0, stmt_create(STMT_BLOCK, 0, 0, 0, 0, $6, 0, 0), 0); }
    | ident TOKEN_COLON type TOKEN_SEMICOLON
        { $$ = decl_create($1, $3, 0, 0, 0); }
    | ident TOKEN_COLON type
        { $$ = decl_create($1, $3, 0, 0, 0); }
    ;

bunch_o_decls: decl bunch_o_decls
                { $1->next = $2; $$ = $1; }
             | 
                { $$ = NULL; }
             ;


/*-----------------------------------------+
| statements                               |
+-----------------------------------------*/
bunch_o_stmts_e: bunch_o_stmts
                 { $$ = $1; }
               |
                 { $$ = NULL; }
               ;

bunch_o_stmts: stmts bunch_o_stmts
                 { $1->next = $2; $$ = $1; }
             | stmts
                 { $$ = $1; }
             ;

stmts: if_stmt
        { $$ = $1; }
     | if_stmt2
        { $$ = $1; }
     ;

if_stmt: TOKEN_IF TOKEN_LEFT_PARENTH expr TOKEN_RIGHT_PARENTH stmts
          { $5->fi = 1; $$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, 0, 0); }
       | TOKEN_IF TOKEN_LEFT_PARENTH expr TOKEN_RIGHT_PARENTH if_stmt2 TOKEN_ELSE if_stmt
          { $5->fi = 1; $7->else_if = 1; $$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, $7, 0); }
       | TOKEN_WHILE TOKEN_LEFT_PARENTH expr TOKEN_RIGHT_PARENTH if_stmt
          { $5->no_indent = 1; $$ = stmt_create(STMT_WHILE, 0, 0, $3, 0, $5, 0, 0); }
       | TOKEN_FOR TOKEN_LEFT_PARENTH arg TOKEN_SEMICOLON arg TOKEN_SEMICOLON arg TOKEN_RIGHT_PARENTH if_stmt
          { $9->no_indent = 1; $$ = stmt_create(STMT_FOR, 0, $3, $5, $7, $9, 0, 0); }
       ;

if_stmt2: TOKEN_IF TOKEN_LEFT_PARENTH expr TOKEN_RIGHT_PARENTH if_stmt2 TOKEN_ELSE if_stmt2
          { $5->fi = 1; $7->else_if = 1; $$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, $7, 0); } 
        | TOKEN_WHILE TOKEN_LEFT_PARENTH expr TOKEN_RIGHT_PARENTH if_stmt2
          { $5->no_indent = 1; $$ = stmt_create(STMT_WHILE, 0, 0, $3, 0, $5, 0, 0); }
        | TOKEN_FOR TOKEN_LEFT_PARENTH arg TOKEN_SEMICOLON arg TOKEN_SEMICOLON arg TOKEN_RIGHT_PARENTH if_stmt2
          { $9->no_indent = 1; $$ = stmt_create(STMT_FOR, 0, $3, $5, $7, $9, 0, 0); }
        | stmt
          { $$ = $1; }
        ;

stmt: expr TOKEN_SEMICOLON
      { $$ = stmt_create(STMT_EXPR, 0, 0, $1, 0, 0, 0, 0); }
    | TOKEN_RETURN arg TOKEN_SEMICOLON
      { $$ = stmt_create(STMT_RETURN, 0, 0, $2, 0, 0, 0, 0); }
    | TOKEN_PRINT expr_list_e TOKEN_SEMICOLON
      { $$ = stmt_create(STMT_PRINT, 0, 0, $2, 0, 0, 0, 0); }
    | TOKEN_LEFT_BRACE bunch_o_stmts_e TOKEN_RIGHT_BRACE
      { $$ = stmt_create(STMT_BLOCK, 0, 0, 0, 0, $2, 0, 0); }
    | decl
      { $$ = stmt_create(STMT_DECL, $1, 0, 0, 0, 0, 0, 0); }
    ;
%%
int yyerror( char *str )
{
	printf("parse error: %s\n",str);
	return 0;
}
