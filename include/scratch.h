#ifndef SCRATCH_H
#define SCRATCH_H

/* definitions */

#define NUM_REGS 7
#define MAX_LABEL_SIZE 30
#define MAX_SYMBOL_SIZE 100

/* structures */

typedef enum {
    LABEL_STRING,
    LABEL_ARRAY,
    LABEL_JUMP
} label_t;

/* reg functions */
int    scratch_reg_alloc(void);
void   scratch_reg_free(int r);
char * scratch_reg_name(int r);

/* label functions */
int    scratch_label_create(label_t type);
char * scratch_label_name(int l, label_t type);

#endif
