#include "scratch.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* globals */

int RegUses         = 0;
int LabelS_count    = 0;
int LabelA_count    = 0;
int LabelJ_count    = 0;
bool Registers[7]  = {true, true, true, true, true, true, true};
char *Reg_names[7] = {"rbx","r10","r11","r12","r13", "r14", "r15"};
char *Arg_regs[6] = {"rdi","rsi","rdx","rcx","r8","r9"};

/* functions */

int scratch_reg_alloc() {
    for (int i = 0; i < NUM_REGS; i++) {
        if(Registers[i]) {
            RegUses++;
            Registers[i] = false;
            return i;
        }
    }
    fprintf(stderr, "Ran out of regs: %d\n", RegUses);
    exit(1);
    return -1;
}

void scratch_reg_free(int r) {
    if(r >= NUM_REGS || r < 0)
        return;
    Registers[r] = true;
}

char *scratch_reg_name(int r) {
    if(r >= NUM_REGS || r < 0)
        return NULL;
    
    return strdup(Reg_names[r]);
}

int scratch_label_create(label_t type) {
    switch(type) {
        case LABEL_STRING:
            return LabelS_count++;
        case LABEL_ARRAY:
            return LabelA_count++;
        case LABEL_JUMP:
            return LabelJ_count++;
    }
}

char *scratch_label_name(int l, label_t type) {
    char buff[MAX_LABEL_SIZE];
    switch (type) {
        case LABEL_STRING:
            sprintf(buff, ".S%d", l);
            break;
        case LABEL_ARRAY:
            sprintf(buff, ".A%d", l);
            break;
        case LABEL_JUMP:
            sprintf(buff, ".L%d", l);
            break;
    }
    return strdup(buff);
}
