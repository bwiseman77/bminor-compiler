#ifndef OPTIONS_H
#define OPTIONS_H
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Definitions */
#define MAX_TOKEN 26
#define ERROR_STRING "scan error:"
#define TOKEN_EOF 0
typedef enum yytokentype token_t;

/* Structures */
typedef struct {
    char command;
    char file[256];
    FILE *fp;
} Options;

/* option functions */
void usage(char *);
bool parse_command_line(int, char **, Options *);

/* scanning functions */
int scan_tokens(FILE *, int print);
void print_token(token_t, char *, int);
void string_literal(char *);
void strchomp(char *);
void char_literal(char *);

/* printer */
int print(void);

/* typechecker */
int resolve(void);

/* resolver */
int typecheck(void);

/* codegen */
int codegen(FILE *, char *);

#endif
