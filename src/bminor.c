#include "token.h"
#include "options.h"
#include "decl.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/* externs */
extern FILE *yyin;
extern int yyparse();
extern struct decl * parser_result;

/* main compiler */
int main(int argc, char *argv[]) {
    // read in commanline args 
    Options options = {0};
    if(!parse_command_line(argc, argv, &options))
        return 1;

    yyin = fopen(options.file, "r");
    if (!yyin) {
        printf("Could not open file!\n");
        usage(argv[0]);
        return 1;
    }
    
    // do the correct option
    switch (options.command) {
       case 's':
           if(scan_tokens(yyin, 1)) {
               puts("scan error");
               exit(1);
           } 
           puts("success");
           break;
       case 'p':
           if(yyparse()) {
               puts("parse error");
               exit(1);
           }
           puts("success");
           break;
       case 'n':
           if(print()) {
               puts("parser or scan error");
               exit(1);
           }
           break;
       case 'r':
           if(resolve()) {
               exit(1);
           }
           break;
       case 't':
           if(typecheck()) { 
               exit(1);
           }
           break;
       case 'c':
           if(codegen(options.fp, options.file)) {
               exit(1);
           }
           break;
       default:
           fprintf(stderr, "bad flag\n");
    }
 
    return EXIT_SUCCESS;
}
