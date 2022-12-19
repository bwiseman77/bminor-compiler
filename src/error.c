#include <stdio.h>
#include "decl.h"
#include "expr.h"
#include "stmt.h"
#include "type.h"
#include "error.h"

/* functions */
void error_print_math(struct expr *e);

/* global error status */
int error_status = 0;

/* global error strings */
char *Error_strings[] = {
    "Error - Cannot NOT non-boolean",                                           // ERR_NOT
    "Error - Cannot negate non-integer",                                        // ERR_NEG
    "Error - Cannot post dec/inc non-integer",                                  // ERR_DEC
    "Error - Condition of ternary must be a boolean",                           // ERR_TERN1
    "Error - Return types of ternary must match",                               // ERR_TERN2
    "Error - Invalid assignment",                                               // ERR_ASSIGN
    "Error - Cannot do math operations on non-integers",                        // ERR_MATH
    "Error - Cannot perform logic operators on non-booleans",                   // ERR_LOGIC
    "Error - Cannot compare non-booleans",                                      // ERR_COMP
    "Error - Cannot test equality of different types",                          // ERR_EQ1
    "Error - Cannot test equality of types: void, function, and array",         // ERR_EQ2
    "Error - Cannot have non-boolean in expression of if statement",            // ERR_IF_BOOL
    "Error - Cannot have non-boolean in expression of while statement",         // ERR_WHILE_BOOL
    "Error - Declaration types and value types must match",                     // ERR_DECL_VAL
    "Error - Return type does not match declaration",                           // ERR_FUNC_TYPE
    "Error - Only function subtypes can be void",                               // ERR_VOID
    "Error - No return value in non-void function",                             // ERR_NO_RETURN 
    "Error - Invalid left-hand assignment",                                     // ERR_LEFT_HAND
    "Error - Not enough args for function call",                                // ERR_EXPR_FEW_ARGS
    "Error - Params do not match",                                              // ERR_EXPR_PARAM_MIS
    "Error - Too many args for function call",                                  // ERR_EXPR_TOO_ARGS
    "Error - Cannot index non-arrays",                                          // ERR_EXPR_BAD_ARR
    "Error - Index of an array must be an integer",                             // ERR_EXPR_BAD_INDEX
    "Error - Arrays cannot have a type void or func",                           // ERR_EXPR_BAD_ARR_TYPE
    "Error - Array literals can only be defined in global scope",               // ERR_EXPR_LIT_GLO
    "Error - Index of array initializer must be integer lit",                   // ERR_EXPR_LIT_IND
    "Error - Array literals do not match",                                      // ERR_EXPR_LIT_MIS
    "Error - Not enough literal values",                                        // ERR_EXPR_LIT_FEW
    "Error - Too many literal values",                                          // ERR_EXPR_LIT_TOO
    "Error - Cannot assign non array to array literal",                         // ERR_DECL_NONARRAY_LIT
    "Error - cannot infer type auto",                                           // ERR_AUTO
    "Error - Global initializers must be constant",                             // ERR_GLOBAL_CONST
    "Error - Cannot have type auto in param list"                               // ERR_AUTO_PARAM
};


/***********************************************************************
 * print an error message given error code ec and two values           *
 ***********************************************************************/
void error_print(error_code ec, void *thing, void *thing2) {
    // increase error count, and set values
    error_status++;
    struct decl *temp;
    struct expr *temp2;
    struct decl *d = thing;
    struct expr *e = thing;
    struct stmt *s = thing;
    struct param_list *p = thing2;
    struct type *t = thing2;
    struct expr *e2 = thing2;

    // set next to null for nicer printing
    if(d) {
        temp = d->next;
        d->next = 0;
    }
   
    // allow for limits since might be annoyong
    if(error_status < 30)
        puts("Dear valued user, I regret to inform you that you made a mistake.");

    // print error string, followed by specifics
    printf("%s:\n", Error_strings[ec]);

    // swicth on error code
    switch (ec) {
        case ERR_VOID:
        case ERR_GLOBAL_CONST:
        case ERR_EXPR_LIT_FEW:
        case ERR_EXPR_LIT_TOO:
        case ERR_AUTO:
        case ERR_DECL_NONARRAY_LIT:
        case ERR_EXPR_LIT_IND:
            decl_print(thing, 1);
            break;
        case ERR_NO_RETURN:
            printf("    For function (%s) with return type ", d->name);
            type_print(d->type->subtype);
            printf("\n");
            break;
        case ERR_EXPR_LIT_GLO:
            decl_print(d, 1);
            break;
        case ERR_EXPR_LIT_MIS:
            printf("    ");
            expr_print(e);
            printf("\n");
            break;
        case ERR_EXPR_PARAM_MIS:
            printf("    ");
            expr_print(e);
            printf(": (%s) expects type ", e->name);
            type_print(p->type);
            printf(" but (");
            expr_print(p->error);
            printf(") is a ");
            type_print(expr_typecheck(p->error));
            printf("\n");
            break;
        case ERR_EXPR_TOO_ARGS:
        case ERR_EXPR_FEW_ARGS:
            printf("    ");
            expr_list_print(e);
            printf(" vs %s(", e->name);
            param_list_print(p);
            printf(")\n");
            break;
        case ERR_FUNC_TYPE:
            printf("    The function (%s) expects return type of ", d->name);
            type_print(d->symbol->type->subtype);
            printf(" but finds a return type of ");
            if(t) type_print(t); else printf("void");
            printf("\n");
            break;
        case ERR_ASSIGN:
            printf("    Cannot assign ");
            expr_print(e->right);
            printf(" which has type ");
            type_print(e->right->error);
            printf(" to ");
            expr_print(e->left);
            printf(" which has type ");
            type_print(e->left->error);
            printf("\n");
            break;
        case ERR_NOT:
            printf("    ");
            expr_print(e);
            printf(" : (");
            expr_print(e->right);
            printf(") is of type ");
            type_print(e->right->error);
            printf("\n");
            break;
         case ERR_DEC:
            printf("    ");
            expr_print(e);
            printf(" : (");
            expr_print(e->left);
            printf(") is of type ");
            type_print(e->left->error);
            printf("\n");
            break;
         case ERR_COMP:
            printf("    ");
            expr_print(e);
            printf(" : (");
            expr_print(e->left);
            printf(") is of type ");
            type_print(e->left->error);
            printf(" while (");
            expr_print(e->right);
            printf(") is of type ");
            type_print(e->right->error);
            printf("\n");
            break;
        case ERR_LEFT_HAND:
            printf("    ");
            expr_print(e);
            printf(" : Can only assign to Idents or Array Expressions, but (");
            expr_print(e->left);
            printf(") is of type ");
            type_print(e->left->error);
            printf("\n");
            break;
        case ERR_WHILE_BOOL:
            printf("    while(");
            expr_print(e);
            printf(") : expects a boolean but finds a ");
            type_print(t);
            printf("\n");
            break;
        case ERR_IF_BOOL:
            printf("    if(");
            expr_print(e);
            printf(") : expects a boolean but finds a ");
            type_print(t);
            printf("\n");
            break;
        case ERR_NEG:
            printf("    ");
            expr_print(e);
            printf(" : (");
            expr_print(e->right);
            printf(") is of type ");
            type_print(e->right->error);
            printf("\n");
            break;
        case ERR_MATH:
            error_print_math(e);
            printf("\n");
            break;
        case ERR_EQ1:
        case ERR_EQ2:
        case ERR_LOGIC:
            printf("    ");
            expr_print(e);
            printf(" : (");
            expr_print(e->left);
            printf(") is of type ");
            type_print(e->left->error);
            printf(" while (");
            expr_print(e->right);
            printf(") is of type ");
            type_print(e->right->error);
            printf("\n");
            break;
        case ERR_TERN1:
            printf("   ");
            expr_print(e->left);
            printf(" : Should be of type boolean, but is of type ");
            type_print(e->left->error);
            printf("\n");
            break;
        case ERR_TERN2:
            printf("   Types should match, but (");
            expr_print(e->mid);
            printf(") is of type ");
            type_print(e->mid->error);
            printf(" while (");
            expr_print(e->right);
            printf(") is of type ");
            type_print(e->right->error);
            printf("\n");
            break;
        case ERR_DECL_VAL:
            printf("    Cannot assign %s which is of type ", d->name);
            type_print(d->type);
            printf(" to ");
            expr_print(d->value);
            printf(" which is of type ");
            type_print(d->value->error);
            printf("\n");
            break;
        case ERR_EXPR_BAD_ARR:
            printf("    ");
            expr_print(e);
            printf(" is not as indexable as you think\n");
            break;
        case ERR_EXPR_BAD_INDEX:
            printf("    ");
            expr_print(e);
            printf(" : (");
            expr_print(e2);
            printf(") should be of type integer, but is of type ");
            type_print(e2->error);
            printf("\n");
            break;
        case ERR_EXPR_BAD_ARR_TYPE:
            decl_print(d, 1);
            break;
        case ERR_AUTO_PARAM:
            s = d->code;
            d->code = 0;
            decl_print(d, 1);
            d->code = s;
            break;
        default:
            break;
     }

    // reset next
    if(d)
        d->next = temp;
    printf("\n");
}

/***********************************************************************
 * Helper for math errors                                              *
 ***********************************************************************/
void error_print_math(struct expr *e) {
    printf("    ");
    expr_print(e);
    printf(" : Cannot ");
    switch (e->kind) {
        case EXPR_ADD:
            printf("add (");
            expr_print(e->right);
            printf(") which is of type ");
            type_print(expr_typecheck(e->right));
            printf(" to (");
            expr_print(e->left);
            printf(") which is of type ");
            type_print(expr_typecheck(e->left));
            break;
        case EXPR_SUB:
            printf("subtract (");
            expr_print(e->right);
            printf(") which is of type ");
            type_print(expr_typecheck(e->right));
            printf(" from (");
            expr_print(e->left);
            printf(") which is of type ");
            type_print(expr_typecheck(e->left));
            break;
        case EXPR_MUL:
            printf("multiply (");
            expr_print(e->left);
            printf(") which is of type ");
            type_print(expr_typecheck(e->left));
            printf(" by (");
            expr_print(e->right);
            printf(") which is of type ");
            type_print(expr_typecheck(e->right));
            break;
        case EXPR_DIV:
            printf("divide (");
            expr_print(e->left);
            printf(") which is of type ");
            type_print(expr_typecheck(e->left));
            printf(" by (");
            expr_print(e->right);
            printf(") which is of type ");
            type_print(expr_typecheck(e->right));
            break;
        case EXPR_MOD:
            printf("mod (");
            expr_print(e->left);
            printf(") which is of type ");
            type_print(expr_typecheck(e->left));
            printf(" by (");
            expr_print(e->right);
            printf(") which is of type ");
            type_print(expr_typecheck(e->right));
            break;
        case EXPR_EX:
            printf("raise (");
            expr_print(e->left);
            printf(") which is of type ");
            type_print(expr_typecheck(e->left));
            printf(" to the power of (");
            expr_print(e->right);
            printf(") which is of type ");
            type_print(expr_typecheck(e->right));
            break;
        default:
            break;
    }
}
