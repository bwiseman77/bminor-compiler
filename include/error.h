#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>
#include <type.h>

/* structures */

typedef enum {
    ERR_NOT,
    ERR_NEG,
    ERR_DEC,
    ERR_TERN1,
    ERR_TERN2,
    ERR_ASSIGN,
    ERR_MATH,
    ERR_LOGIC,
    ERR_COMP,
    ERR_EQ1,
    ERR_EQ2,
    ERR_IF_BOOL,
    ERR_WHILE_BOOL,
    ERR_DECL_VAL,
    ERR_FUNC_TYPE,
    ERR_VOID,
    ERR_NO_RETURN,
    ERR_LEFT_HAND,
    ERR_EXPR_FEW_ARGS,
    ERR_EXPR_PARAM_MIS,
    ERR_EXPR_TOO_ARGS,
    ERR_EXPR_BAD_ARR,
    ERR_EXPR_BAD_INDEX,
    ERR_EXPR_BAD_ARR_TYPE,
    ERR_EXPR_LIT_GLO,
    ERR_EXPR_LIT_IND,
    ERR_EXPR_LIT_MIS,
    ERR_EXPR_LIT_FEW,
    ERR_EXPR_LIT_TOO,
    ERR_DECL_NONARRAY_LIT,
    ERR_AUTO,
    ERR_GLOBAL_CONST,
    ERR_AUTO_PARAM
} error_code;

struct error {
    struct type *return_type;
    int return_code;
};

/* functions */

void error_print(error_code ec, void *thing, void *thing2);

#endif
