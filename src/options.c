#include "options.h"
#include "decl.h"
#include <string.h>

/* externs */
extern char *yytext;
extern int yylex();
extern int yyparse();
extern struct decl * parser_result;
extern int error_status;

/* definitions */
#define ERROR_STRING "scan error:"

/* usage message for program */
void usage(char *program) {
    fprintf(stderr, "Usage: %s [options] $FILE\n\n", program);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "   -scan       File to scan\n");
    fprintf(stderr, "   -parse      File to parse\n");
    fprintf(stderr, "   -print      File to print\n");
    fprintf(stderr, "   -resolve    File to resolve\n");
    fprintf(stderr, "   -typecheck  File to typecheck\n");
    fprintf(stderr, "   -codegen    File to codegen followed by file name of dest\n");
    fprintf(stderr, "   -h          Print help message\n");
}

/* parse command line arguments and store in options struct */
bool parse_command_line(int argc, char *argv[], Options *options) {
    int argind = 1;

    while (argind < argc && strlen(argv[argind]) > 1 && argv[argind][0] == '-') {
        char *arg = argv[argind++]; 

        if (!strcmp(arg, "-scan")) {
            options->command = 's';

        } else if (!strcmp(arg, "-parse")) {
            options->command = 'p';
                
        } else if (!strcmp(arg, "-print")) {
            options->command = 'n';
 
        } else if (!strcmp(arg, "-resolve")) {
            options->command = 'r';
        
        } else if (!strcmp(arg, "-typecheck")) {
            options->command = 't';
 
        } else if (!strcmp(arg, "-codegen")) {
            options->command = 'c';
            if(argc != 4) {
                usage(argv[0]);
                return false;
            }
            options->fp = fopen(argv[3], "w+");
            if (!options->fp) {
                fprintf(stderr, "Count not open file %s", argv[3]);
                return false;
            }
            
        
        } else if (!strcmp(arg, "-h")) {
            usage(argv[0]);
            return false;

        } else {
            usage(argv[0]);
            return false;
        }
    }

    if(argind >= argc) {
        usage(argv[0]);
        return false;
    }

    strcpy(options->file, argv[argind]);

    return true;
}

/* go through and rm " and backslash (char) from strings */
void string_literal(char *string) {
    // replace 
    char *newstring = string;
    char *c = string;

    while(*c) { 
        // dont add backwacks
        if(*c != '\\' && *c != '\"') {
            *newstring = *c;
            newstring++;
            c++;
        } 

        // if backwack, get thing after it
        else if (*c == '\\') {
            c++;
            // if n or 0, means special, so add both
            if (*c == 'n') {
                *newstring = '\n';
               } else if (*c == '0') {
                *newstring = '\0';
             // if null, quit
            } else if (*c == '\0') {
                return;
            // if not special, just add
            } else {
                *newstring = *c;
             }
            // now move
            newstring++;
            c++;
        } else {
            // just move
            c++; 
        }
    } 
    // terminate
    *newstring = '\0';
}

/* get rid of ' and backslash (char) from character */
/* must assume len(c) at least 3 */
void char_literal(char *c) {

    // check for backslash
    if(*(c+1) == '\\')

        // if n or 0, put special, else just put thing between 
        if(*(c+2) == 'n')
            *c = '\n';
        else if(*(c+2) == '0')
            *c = '\0';
        else
            *c = *(c+2);
    else
        *c = *(c+1);

    // terminate
    *(c+1) = '\0';
}

/* get rid of newline */
void strchomp(char *string) {
    int len = strlen(string);
    *(string+len-1) = '\0';
}

/* Print tokens */
void print_token(token_t token, char *test, int bad) {
        char *t;
        if(token == TOKEN_EOF)
            t = "EOF";
        else if(token == TOKEN_ERROR)
            t = "SCAN ERROR";
        else if(token == TOKEN_BAD_STRING)
            t = "INVALID STRING LITERAL";
        else if(token == TOKEN_BAD_COMMENT)
            t = "INVALID COMMENT";
        else if(token == TOKEN_BAD_CHAR)
            t = "INVALID CHAR LITERAL";
        else if(token == TOKEN_BAD_IDENTIFIER)
            t = "INVALID IDENTIFER";

        // whitespace
        else if(token == TOKEN_MULTI_LINE)
            t = "MULTI_LINE_COMMENT";
        else if(token == TOKEN_SINGLE_LINE)
            t = "SINGLE_LINE_COMMENT";
        else if(token == TOKEN_WHITESPACE)
            t = "WHITESPACE";
    
        // keywords
        else if(token == TOKEN_ARRAY)
            t = "ARRAY";
        else if(token == TOKEN_AUTO)
            t = "AUTO";
        else if(token == TOKEN_BOOLEAN)
            t = "BOOLEAN";
        else if(token == TOKEN_CHAR)
            t = "CHAR";
        else if(token == TOKEN_ELSE)
            t = "ELSE";
        else if(token == TOKEN_FALSE)
            t = "FALSE";
        else if(token == TOKEN_FOR)
            t = "FOR";
        else if(token == TOKEN_FUNCTION)
            t = "FUNCTION";
        else if(token == TOKEN_IF)
            t = "IF";
        else if(token == TOKEN_INTEGER)
            t = "INTEGER";
        else if(token == TOKEN_PRINT)
            t = "PRINT";
        else if(token == TOKEN_RETURN)
            t = "RETURN";
        else if(token == TOKEN_STRING)
            t = "STRING";
        else if(token == TOKEN_TRUE)
            t = "TRUE";
        else if(token == TOKEN_VOID)
            t = "VOID";
        else if(token == TOKEN_WHILE)
            t = "WHILE";

        // expressions
        else if(token == TOKEN_ADD)
            t = "ADD";
        else if(token == TOKEN_SUB)
            t = "SUB";
        else if(token == TOKEN_ASSIGN)
            t = "ASSIGN";
        else if(token == TOKEN_INCREMENT)
            t = "INCREMENT";
        else if(token == TOKEN_DECREMENT)
            t = "DECREMENT";
        else if(token == TOKEN_NOT)
            t = "NOT";
        else if(token == TOKEN_EXPONENT)
            t = "EXPONENT";
        else if(token == TOKEN_MULTIPLICATION)
            t = "MULTIPLICATION";
        else if(token == TOKEN_DIVISON)
            t = "DIVISON";
        else if(token == TOKEN_MODULO)
            t = "MODULO";
        else if(token == TOKEN_LT)
            t = "LT";
        else if(token == TOKEN_LTE)
            t = "LTE";
        else if(token == TOKEN_GT)
            t = "GT";
        else if(token == TOKEN_GTE)
            t = "GTE";
        else if(token == TOKEN_EQ)
            t = "EQ";
        else if(token == TOKEN_NOT_EQ)
            t = "NOT_EQ";
        else if(token == TOKEN_AND)
            t = "AND";
        else if(token == TOKEN_OR)
            t = "OR";
        else if(token == TOKEN_TERNARY_PT_1)
            t = "TERNARY_PT_1";
    
        // literals
        else if(token == TOKEN_IDENT)
            t = "IDENT";
        else if(token == TOKEN_CHAR_LITERAL)
            t = "CHAR_LITERAL";
        else if(token == TOKEN_STRING_LITERAL)
            t = "STRING_LITERAL"; 
        else if(token == TOKEN_INTEGER_LITERAL)
            t = "INTEGER_LITERAL";
    
        // misc
        else if(token == TOKEN_COLON)
            t = "COLON";
        else if(token == TOKEN_SEMICOLON)
            t = "SEMICOLON";
        else if(token == TOKEN_COMMA)
            t = "COMMA";
        else if(token == TOKEN_SINGLE_QUOTE)
            t = "SINGLE_QUOTE";
        else if(token == TOKEN_DOUBLE_QUOTE)
            t = "DOUBLE_QUOTE"; 
        else if(token == TOKEN_LEFT_BRACE)
            t = "LEFT_BRACE";
        else if(token == TOKEN_RIGHT_BRACE)
            t = "RIGHT_BRACE";
        else if(token == TOKEN_LEFT_BRACKET)
            t = "LEFT_BRACKET";
        else if(token == TOKEN_RIGHT_BRACKET)
            t = "RIGHT_BRACKET";
        else if(token == TOKEN_LEFT_PARENTH)
            t = "LEFT_PARENTH";
        else if(token == TOKEN_RIGHT_PARENTH)
            t = "RIGHT_PARENTH";

    if(bad) {
        fprintf(stdout, "%20s  %s\n", t, yytext);
    } else {
        fprintf(stderr, "%20s  %s\n", t, yytext);
    }
}
/* scanner comiler option s */
int scan_tokens(FILE *file_in, int print) {
    printf("-------------------------------------------\n");
    printf("          token type  text\n");
    printf("-------------------------------------------\n");

    token_t t;
    int bad;
    while (1) {
        t = yylex();
        bad = 0;
        if(t == TOKEN_EOF) break;

        if (t == TOKEN_ERROR ||
                    t == TOKEN_BAD_STRING ||
                    t == TOKEN_BAD_COMMENT ||
                    t == TOKEN_BAD_CHAR) {
            bad = 1;
        } else if (t == TOKEN_STRING_LITERAL) {
            string_literal(yytext);
            if(strlen(yytext) > 256) {
                t = TOKEN_BAD_STRING;
                bad = 1;
            }
        } else if (t == TOKEN_CHAR_LITERAL) {
            char_literal(yytext);
        } else if (t == TOKEN_SINGLE_LINE) {
            strchomp(yytext);
        } else if (t == TOKEN_MULTI_LINE) {
            *yytext = '\0'; 
        } else if (t == TOKEN_IDENT) {
            if(strlen(yytext) > 256) {
                t = TOKEN_BAD_IDENTIFIER;
                bad = 1;
            }
        }

        if(print)
            print_token(t, yytext, bad);

        if(bad)
            return 1;
    }

    return 0;
}


/* printer compiler option */
int print() {
    int status = yyparse();
    decl_print(parser_result, 0);
    return status;
}

/* resolver compiler option */
int resolve() {
    struct scope *s = scope_create();
    int status = yyparse();
    status += decl_resolve(s, parser_result);
    return status;
}

/* typechecker compiler option */
int typecheck() {
    struct scope *s = scope_create();
    if(yyparse()) 
        return 1;
    puts("RESOLVING:"); 
    if(decl_resolve(s, parser_result))
        return 1;
        
    puts("TYPECHECKING:");
    decl_typecheck(parser_result);
    if(!error_status)
        puts("No type errors");

    return error_status;
}

/* codegen compiler option */
int codegen(FILE *fp, char *file) {
    if(!typecheck()) {
        fprintf(fp, ".file \"%s\"\n", file);
        decl_codegen(parser_result, fp);
    }
    return error_status;
}
