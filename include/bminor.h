#ifndef BMINOR_H
#define BMINOR_H
#include <stdio.h>

#define MAX_TOKEN 26


/* scanner functions */
int scan_tokens(FILE *f);
void string_literal(char *s);
void strchomp(char *s);
void char_literal(char *c);

#endif
