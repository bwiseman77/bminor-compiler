#include "expr.h"
#include "symbol.h"
#include "type.h"

#include "error.h"
#include <stdlib.h>
#include <stdio.h>

/************************************************************************
 * creates an expr struct and returns it                                *
 ***********************************************************************/
struct expr * expr_create(expr_t kind, struct expr *left, struct expr *right) {
    struct expr *e = malloc(sizeof(struct expr));
    if (!e)
        return NULL;

    e->kind = kind;
    e->left = left;
    e->right = right;
    e->group = 0;
    e->loop_expr = 0;
    e->func_value = 0;
    return e;
}

/************************************************************************
 * creates an expr with a given name                                    *
 ***********************************************************************/
struct expr * expr_create_name( const char *n ) {
    struct expr *e = expr_create(EXPR_IDENT, 0, 0);
    e->name = n;
    return e;
}

/************************************************************************
 * creates an expr struct with given int                                *
 ***********************************************************************/
struct expr * expr_create_integer_literal( int c ) {
    struct expr *e = expr_create(EXPR_INT, 0, 0);
    e->literal_value = c;
    return e;
}

/************************************************************************
 * creates an expr struct with a given int (treated as bool)            *
 ***********************************************************************/
struct expr * expr_create_boolean_literal( int c ) {
    struct expr *e = expr_create(EXPR_BOOL, 0, 0);
    e->literal_value = c;
    return e;
}

/************************************************************************
 * creates an expr struct with a given string                           *
 ***********************************************************************/
struct expr * expr_create_string_literal( const char *str ) {
    struct expr *e = expr_create(EXPR_STR, 0, 0);
    e->string_literal = str;
    return e;
}

/************************************************************************
 * creates an expr struct with a given string                           *
 ***********************************************************************/
struct expr * expr_create_char_literal( const char *str ) {
    struct expr *e = expr_create(EXPR_CHAR, 0, 0);
    e->string_literal = str;
    return e;
}

 /************************************************************************
 * creates an expr struct for an array subscript.                        *
 * Puts ptr of expr list in mid                                          *
 *************************************************************************/

struct expr * expr_create_array_subscript( const char *n, expr_t kind, struct expr *mid) {
    struct expr *e = expr_create(kind, 0, 0);
    e->mid = mid;
    e->name = n;
    return e;
}
/************************************************************************
 * creates an expr struct for a function call.                          *
 * Puts ptr of expr list in mid                                         *
 ************************************************************************/

struct expr * expr_create_func_call( const char *n, expr_t kind, struct expr *mid) {
    struct expr *e = expr_create(kind, 0, 0);
    e->mid = mid;
    e->name = n;
    return e;
}
/************************************************************************
 * Creates an expr struct for a tern                                    *
 ************************************************************************/
struct expr * expr_create_tern( struct expr * left, struct expr * mid, struct expr * right) {
    struct expr *e = expr_create(EXPR_TERN, left, right);
    e->mid = mid;

    return e;
}

/************************************************************************
 * Creates an struct for an array literal, puts ptr to expr list in mid *
 ************************************************************************/
struct expr * expr_create_array_literal( struct expr *a) {
    struct expr *e = expr_create(EXPR_ARRAY_LIT, 0, 0);
    e->mid = a;
    return e;
}

/************************************************************************
 * Helper function to check precedence. Enums were set to have same     *
 * precedence at same 10s digit, so dividing by 10 reveals precednce.   *
 * Uses group memeber if expr was given (), and if same precedent,      *
 * keeps them. Returns 1 if needs () and 0 if doesnt.                   *
 ***********************************************************************/
int check_precedence(struct expr *a, struct expr *b, int r) {
    if(a->kind == EXPR_ASSIGN || a->kind == EXPR_EX)
        r = !r;
    
    if(a->kind/10 > b->kind/10)
        return 0;
    if(a->kind/10 == b->kind/10)
        if (a->group && r)
            return 1;
        else
            return 0;
    else
        return 1;
}

/***********************************************************************
 * Prints an expr. Special print for tern. prints () based on check    *
 * precedence function. Returns if e is NULL.                          *
 ***********************************************************************/
void expr_print(struct expr *e) {
    if(!e) return;
    
    /* special tern case */
    if(e->kind == EXPR_TERN) {
        print_expr_t(e);
        return;
    }
    
    /* print left */
    if(e->left) {
        if(check_precedence(e->left, e, 0)) {
            printf("(");
            expr_print(e->left);
            printf(")");
        } else {
            expr_print(e->left);
        }
    }
      
    /* print current */
    print_expr_t(e);
              
    /* print right */
    if(e->right) {
        if(check_precedence(e->right, e, 1)) {
            printf("(");
            expr_print(e->right);
            printf(")");
        } else {
            expr_print(e->right);
        }
    }

}

/***********************************************************************
 * Prints an expr array literal. Prints an eprx list for args          * 
 ***********************************************************************/
void expr_print_array_lit(struct expr *e) {
    if(!e) return;
    printf("{");
    expr_print(e);
    struct expr *t = e->next;
    while(t) {
        printf(", ");
        expr_print(t);
        t = t->next;
    };
    printf("}");
}

/***********************************************************************
 * Prints an expr list.                                                *
 ***********************************************************************/
void expr_list_print(struct expr *e) {
    struct expr *t = e;
    while(t) {
        expr_print(t);
        if(t->next)
            printf(", ");
        t = t->next;
    }
}

/***********************************************************************
 * Prints an expr array args                                           *
 ***********************************************************************/
void expr_array_print(struct expr *e) {
    if(!e) return;
    printf("[");
    expr_print(e);
    printf("]");
    expr_array_print(e->next);
}

/***********************************************************************
 * Prints specific expr_ts                                             *
 ***********************************************************************/
void print_expr_t (struct expr *e) {
    switch (e->kind) {
        case EXPR_STR:
            printf("%s", e->string_literal);
            break;
        case EXPR_CHAR:
            printf("%s", e->string_literal);
            break;
        case EXPR_IDENT:
            printf("%s", e->name);
            break;
        case EXPR_INT:
            printf("%d", e->literal_value);
            break;
        case EXPR_BOOL:
            if(e->literal_value)
                printf("true");
            else
                printf("false");
            break;
        case EXPR_SUB:
            printf("-");
            break;
        case EXPR_MUL:
            printf("*");
            break;
        case EXPR_DIV:
            printf("/");
            break;
        case EXPR_LT:
            printf("<");
            break;
        case EXPR_LTE:
            printf("<=");
            break;
        case EXPR_GT:
            printf(">");
            break;
        case EXPR_GTE:
            printf(">=");
            break;
        case EXPR_EQ:
            printf("==");
            break;
        case EXPR_NEQ:
            printf("!=");
            break;
        case EXPR_EX:
            printf("^");
            break;
        case EXPR_MOD:
            printf("%%");
            break;
        case EXPR_INC:
            printf("++");
            break;
        case EXPR_DEC:
            printf("--");
            break;
        case EXPR_NOT:
            printf("!");
            break;
        case EXPR_AND:
            printf("&&");
            break;
        case EXPR_OR:
            printf("||");
            break;
        case EXPR_ASSIGN:
            printf("=");
            break;
        case EXPR_NEG:
            printf("-");
            break;
        case EXPR_TERN:
            if(e->left)
                if(e->left->group) printf("(");
            expr_print(e->left);            
            if(e->left)
                if(e->left->group) printf(")");

            printf("?");

            if(e->mid)
                if(e->mid->group) printf("(");
            expr_print(e->mid);
            if(e->mid)
                if(e->mid->group) printf(")");

            printf(":");
            expr_print(e->right);
            break;
        case EXPR_FUNC:
            printf("%s(", e->name);
            expr_list_print(e->mid);
            printf(")");
            break;
        case EXPR_ARRAY_LIT:
            expr_print_array_lit(e->mid);
            break;
        case EXPR_ARRAY:
            printf("%s", e->name);
            expr_array_print(e->mid);
            break;
        case EXPR_GROUP:
            expr_print(e->mid);
            break;
        case EXPR_ADD:
            printf("+");
            break;
    }
}

/************************************************************************
 * helper function to check all parts of expr after doing main expr    *
 ***********************************************************************/
int exprs_resolve(struct scope *s, struct expr *e) {
    int status = 0;
    status += expr_resolve(s, e->left);
    status += expr_resolve(s, e->mid);
    status += expr_resolve(s, e->right);
    status += expr_resolve(s, e->next);
    return status;
}

/************************************************************************
 * main expr resolve function. creates symbol if needed and returns     *
 * error status. calls helper to check all other parts                  *
 ***********************************************************************/
int expr_resolve(struct scope *s, struct expr *e) {
    int status = 0;
    if(!e) return status;

    // only for names
    if(e->kind == EXPR_IDENT || e->kind == EXPR_FUNC || e->kind == EXPR_ARRAY) {
        struct symbol *sym = scope_lookup(s, e->name);

        // check if worked
        if(sym) {
            e->symbol = sym;
            symbol_print(sym);
        } else {
            printf("resolve error: %s is not defined\n", e->name);
            status = 1;
        }
        
    }

    status += exprs_resolve(s, e);
    return status;
}

/************************************************************************
 * checks a function call to make sure matches param_list               *
 ***********************************************************************/
void expr_func_check(struct expr *e_list, struct param_list *p_list) {
    // e->mid points to first arg
    struct expr *e = e_list->mid;

    // loop through param list to check since we need exactly these args
    for (struct param_list *p = p_list; p; p = p->next) { 

        // if e is null, means we are out of args when expected more, so too few args given
        if(!e) {
            error_print(ERR_EXPR_FEW_ARGS, e_list, p_list);
            return;
        }

        // if we find an arg, check to make sure it matches type of param
        if(!type_equals(expr_typecheck(e), p->type)) {
            p->error = e;
            error_print(ERR_EXPR_PARAM_MIS, e_list, p);
        }

        // if we have an arg, check for next arg
        if(e)
            e = e->next;
    }

    // if we get through all params, but still have args, too many args were given
    if(e)
        error_print(ERR_EXPR_TOO_ARGS, e_list, p_list);
}

/************************************************************************
 * typechecks an array subscript                                        *
 ***********************************************************************/
struct type * expr_array_typecheck(struct expr *e) {
    // mid points to first index
    struct expr *curr = e->mid;
    // get a copy of subtype
    struct type *sub_type = type_copy(e->symbol->type);
    struct type *t;

    // while we have indexes, check them
    while(curr) {

         // if current level is not an array, error
         if(sub_type->kind != TYPE_ARRAY) {

            error_print(ERR_EXPR_BAD_ARR, e, curr);
            return sub_type;
        }
        
        // get type of index and check it is an int
        t = expr_typecheck(curr);
        curr->error = t;
        if(t->kind != TYPE_INTEGER)
            error_print(ERR_EXPR_BAD_INDEX, e, curr);//

        // get next subtype
        sub_type = type_copy(sub_type->subtype);
        
        // make sure next subtype is not void or function
        if(sub_type->kind == TYPE_VOID || sub_type->kind == TYPE_FUNCTION)
            error_print(ERR_EXPR_BAD_ARR_TYPE, e, 0);//

        // get next index
        curr = curr->next; 
    }
    return sub_type;
}

/***********************************************************************
 * copy an expr                                                        *
 ***********************************************************************/
struct expr * expr_copy(struct expr *e) {
    if(!e) return 0;

    struct expr *t = expr_create(e->kind, expr_copy(e->left), expr_copy(e->right));
    t->literal_value = e->literal_value;
    return t;
}

/***********************************************************************
 * typecheck an expr                                                   *
 ***********************************************************************/
struct type * expr_typecheck(struct expr *e) {
    if(!e) return 0;

    struct type *lt = expr_typecheck(e->left);
    // for error printing
    if(e->left)
        e->left->error = type_copy(lt);
    struct type *mt;
    struct type *rt = expr_typecheck(e->right);
    // for error printing
    if(e->right)
        e->right->error = type_copy(rt);

    struct type *result;
    switch (e->kind) {
        // literals
        case EXPR_INT:
             result = type_create(TYPE_INTEGER, 0, 0);
            break;
        case EXPR_BOOL:
            result = type_create(TYPE_BOOLEAN, 0, 0);
            break;
        case EXPR_IDENT:
            result = type_copy(e->symbol->type);
            break;
        case EXPR_CHAR:
            result = type_create(TYPE_CHARACTER, 0, 0);
            break;
        case EXPR_STR:
            result = type_create(TYPE_STRING, 0, 0);
            break;
        case EXPR_FUNC:
            expr_func_check(e, e->symbol->type->params);
            result = type_copy(e->symbol->type->subtype);
            break;
        case EXPR_GROUP:
            result = expr_typecheck(e->mid);
            break;
        case EXPR_ARRAY:
            result = expr_array_typecheck(e);
            break;
        case EXPR_ARRAY_LIT:
            result = type_literal_convert(e);
            break;

        // only rt
        case EXPR_NOT:
            if(rt->kind != TYPE_BOOLEAN)
                error_print(ERR_NOT, e, 0);//
            
            result = type_copy(rt);

            break;
        case EXPR_NEG:
            if(rt->kind != TYPE_INTEGER)
                error_print(ERR_NEG, e, 0);//

            result = type_copy(rt);
            break;

        // only lt
        case EXPR_INC:
        case EXPR_DEC:
            if(lt->kind != TYPE_INTEGER && e->left->kind != EXPR_IDENT && e->left->kind != EXPR_ARRAY)
                error_print(ERR_DEC, e, 0);//
            
            result = type_copy(lt);
            break;

        // special
        case EXPR_TERN:
            mt = expr_typecheck(e->mid);
            
            e->mid->error = type_copy(mt);
            if(!(lt->kind == TYPE_BOOLEAN))
                error_print(ERR_TERN1, e, 0);     
            if(!type_equals(mt, rt))
                error_print(ERR_TERN2, e, 0);//
            
            result = type_copy(mt);
            break;
        case EXPR_ASSIGN:
            // retype auto if possible 
            if(e->left && e->left->symbol && e->left->symbol->type->kind == TYPE_AUTO) {
                if(rt && rt->kind == TYPE_AUTO) {
                    error_print(ERR_AUTO, e, 0);
                    puts("error");

                } else { 
                    printf("notice: type of %s is ", e->left->name);
                
                     e->left->symbol->type= type_copy(rt);
                     type_print(e->left->symbol->type);
                     printf("\n");
                }
                
                result = type_copy(rt);     

            // only can assign idents and arrays
            } else if (e->left->kind != EXPR_IDENT && e->left->kind != EXPR_ARRAY) {
                error_print(ERR_LEFT_HAND, e, 0);

            // need to be mathcing types
            } else if (type_equals(lt, rt)) {
                 result = type_copy(lt);

            // default
            } else {
                error_print(ERR_ASSIGN, e, 0);//
                result = type_copy(lt);
            }
            break;

        // only integers
        case EXPR_ADD:
        case EXPR_SUB:
        case EXPR_MUL:
        case EXPR_DIV:
        case EXPR_MOD:
        case EXPR_EX:
            if(!(lt->kind == TYPE_INTEGER && rt->kind == TYPE_INTEGER && type_equals(lt, rt)))
                error_print(ERR_MATH, e, 0);//
            
            result = type_copy(lt);
            break;

        // logical: bool -> bool
        case EXPR_OR:
        case EXPR_AND:
            if(!(lt->kind == TYPE_BOOLEAN && rt->kind == TYPE_BOOLEAN && type_equals(lt, rt)))
                error_print(ERR_LOGIC, e, 0);
                
            result = type_copy(lt);
            break;

        // compare: int -> bool
        case EXPR_LT:
        case EXPR_LTE:
        case EXPR_GT:
        case EXPR_GTE:
            if(!(lt->kind == TYPE_INTEGER && rt->kind == TYPE_INTEGER && type_equals(lt, rt)))
                error_print(ERR_COMP, e, 0);
                
            result = type_create(TYPE_BOOLEAN, 0, 0);
            break;

        // compare: !void, array, func
        case EXPR_EQ:
        case EXPR_NEQ:
            if(lt->kind != TYPE_VOID && lt->kind != TYPE_ARRAY && lt->kind != TYPE_FUNCTION && 
               rt->kind != TYPE_VOID && rt->kind != TYPE_ARRAY && rt->kind != TYPE_FUNCTION) {
                if (!type_equals(lt, rt))
                    error_print(ERR_EQ1, e, 0);
            } else {
                error_print(ERR_EQ2, e, 0);
            }

            result = type_create(TYPE_BOOLEAN, 0, 0);
            break;
    }
 
    return result;
}

/***********************************************************************
 * helper to codegen an func call                                      *
 ***********************************************************************/
void expr_func_codegen(struct expr *e, FILE *out) {
    extern char *Arg_regs[6];
    struct expr *curr = e->mid;
    int count = 0;

    // save responible regs
    fprintf(out, "# save regs for func call\n");
    fprintf(out, "\tpushq %%r10\n");
    fprintf(out, "\tpushq %%r11\n");

    // put args in temps
    fprintf(out, "# putting args in temps\n");
    while(curr) {
        expr_codegen(curr, out);
        curr = curr->next;
    }

    // put args in arg regs
    curr = e->mid;
    fprintf(out, "# load args into arg registers\n");
    while(curr) {
        fprintf(out, "\tmovq %%%s, %%%s\n", 
                scratch_reg_name(curr->reg), 
                Arg_regs[count]);
        scratch_reg_free(curr->reg);
        count++;
        curr = curr->next;
    }

    // call func
    fprintf(out, "# call function\n");
    fprintf(out, "\tcall %s\n", e->name);

    // restore regs
    fprintf(out, "# restore regs after func call\n");
    fprintf(out, "\tpopq %%r11\n");
    fprintf(out, "\tpopq %%r10\n");

    // put result in rax?
    // e->func_value
    if(e->symbol->type->subtype->kind != TYPE_VOID) {
        fprintf(out, "# load return into scratch reg\n");
        e->reg = scratch_reg_alloc();
        fprintf(out, "\tmovq %%rax, %%%s\n", scratch_reg_name(e->reg));
    }
        
}

/***********************************************************************
 * chelper to odegen a cmp expr                                        *
 ***********************************************************************/
void expr_cmp_codegen(struct expr *e, FILE *out) {
    
    int l1, l2, l3;
    fprintf(out, "# generating code for cmp\n");
    expr_codegen(e->left, out);
    expr_codegen(e->right, out);
    e->reg = e->right->reg;
    fprintf(out, "# doing the cmp\n");

    if(e->left && e->left->symbol && e->left->symbol->type->kind == TYPE_STRING) { 
        fprintf(out, "\tpushq %%r10\n");
        fprintf(out, "\tpushq %%r11\n");
        fprintf(out, "\tmovq %%%s, %%rdi\n", scratch_reg_name(e->left->reg));
        fprintf(out, "\tmovq %%%s, %%rsi\n", scratch_reg_name(e->right->reg));
        fprintf(out, "\tcall string_compare\n");
        fprintf(out, "\tpopq %%r11\n");
        fprintf(out, "\tpopq %%r10\n");
        fprintf(out, "\tcmpq $1, %%rax\n"); 
    } else {
        fprintf(out, "\tcmpq %%%s, %%%s\n", scratch_reg_name(e->right->reg), scratch_reg_name(e->left->reg));
    }
    // l1: if true label
    // l2: if false label
    // l3: continue label
    l1 = scratch_label_create(LABEL_JUMP);
    e->label = l1;

    switch (e->kind) {
        case EXPR_LT:
            fprintf(out, "\tjl %s\n", scratch_label_name(l1, LABEL_JUMP));
            break;
        case EXPR_LTE:
            fprintf(out, "\tjle %s\n", scratch_label_name(l1, LABEL_JUMP));
            break;
        case EXPR_GT:
            fprintf(out, "\tjg %s\n", scratch_label_name(l1, LABEL_JUMP));
            break;
        case EXPR_GTE:
            fprintf(out, "\tjge %s\n", scratch_label_name(l1, LABEL_JUMP));
            break;
        case EXPR_EQ:
            fprintf(out, "\tje %s\n", scratch_label_name(l1, LABEL_JUMP));
            break;
        case EXPR_NEQ:
            fprintf(out, "\tjne %s\n", scratch_label_name(l1, LABEL_JUMP));
            break;
    }
   

    l2 = scratch_label_create(LABEL_JUMP);
    l3 = scratch_label_create(LABEL_JUMP);
    // false
    fprintf(out, " # if false\n");
    fprintf(out, "%s:\n\tmovq $0, %%%s\n\tjmp %s\n", 
        scratch_label_name(l2, LABEL_JUMP), 
        scratch_reg_name(e->reg),
        scratch_label_name(l3, LABEL_JUMP));
            
    // true
    fprintf(out, " # if true\n");
    fprintf(out, "%s:\n\tmovq $1, %%%s\n\tjmp %s\n", 
        scratch_label_name(l1, LABEL_JUMP), 
        scratch_reg_name(e->reg),
        scratch_label_name(l3, LABEL_JUMP));

    // cont
    fprintf(out, " # continue\n");
    fprintf(out, "%s:\n", scratch_label_name(l3, LABEL_JUMP));

    scratch_reg_free(e->left->reg);
}

/***********************************************************************
 * codegen an expr                                                     *
 ***********************************************************************/
void expr_codegen(struct expr *e, FILE *out) {
    if(!e) return;

    int l1, l2, l3;
    switch (e->kind) {
        // literals
        case EXPR_INT:
            e->reg = scratch_reg_alloc();
            fprintf(out, "\tmovq $%d, %%%s\n", e->literal_value, scratch_reg_name(e->reg)); 
            break;
        case EXPR_BOOL:
            e->reg = scratch_reg_alloc();
            fprintf(out, "\tmovq $%d, %%%s\n", e->literal_value, scratch_reg_name(e->reg));
            break;
        case EXPR_IDENT:
            e->reg = scratch_reg_alloc();
            fprintf(out, "\tmovq %s, %%%s\n", symbol_codegen(e->symbol), scratch_reg_name(e->reg));
            break;
        case EXPR_CHAR:
            e->reg = scratch_reg_alloc();
           
            if(*(e->string_literal+1) != '\\') {
                fprintf(out, "\tmovq $%d, %%%s\n", *(e->string_literal+1), scratch_reg_name(e->reg));
            } else {
                if(*(e->string_literal+2) == 'n')
                    fprintf(out, "\tmovq $%d, %%%s\n", '\n', scratch_reg_name(e->reg));
                else if(*(e->string_literal+2) == '\\')
                    fprintf(out, "\tmovq $%d, %%%s\n", '\\', scratch_reg_name(e->reg));
                else if(*(e->string_literal+2) == '0')
                    fprintf(out, "\tmovq $%d, %%%s\n", '\0', scratch_reg_name(e->reg));
                else
                    fprintf(out, "\tmovq $%d, %%%s\n", *(e->string_literal+2), scratch_reg_name(e->reg));

            }

            break;
        case EXPR_STR:
            l1 = scratch_label_create(LABEL_STRING);
 
            // make new string lit
            fprintf(out, ".data\n%s:\n\t.string %s\n.text\n", 
                    scratch_label_name(l1, LABEL_STRING),
                    e->string_literal);

            // put address in reg
            e->reg = scratch_reg_alloc();
            fprintf(out, "\tleaq %s, %%%s\n", scratch_label_name(l1, LABEL_STRING), scratch_reg_name(e->reg));
            break;
        case EXPR_FUNC:
            expr_func_codegen(e, out);
            break;
        case EXPR_GROUP:
            expr_codegen(e->mid, out);
            break;
        case EXPR_ARRAY:
            fprintf(out, " # indexing array\n");
            e->reg = scratch_reg_alloc();
            expr_codegen(e->mid, out);

            fprintf(out, "\tleaq %s, %%%s\n", symbol_codegen(e->symbol), scratch_reg_name(e->reg));
            fprintf(out, "\tmovq %%%s, %%rax\n", scratch_reg_name(e->mid->reg));
            scratch_reg_free(e->mid->reg);

            int ri = scratch_reg_alloc();
            fprintf(out, "\tmovq $8, %%%s\n", scratch_reg_name(ri));
            fprintf(out, "\timulq %%%s\n", scratch_reg_name(ri));
            scratch_reg_free(ri);

            fprintf(out, "\taddq %%%s, %%rax\n", scratch_reg_name(e->reg));
            fprintf(out, "\tmovq (%%rax), %%%s\n", scratch_reg_name(e->reg));
            break;
        case EXPR_ARRAY_LIT:
            l1 = scratch_label_create(LABEL_ARRAY);

            // make new array

            fprintf(out, ".data\n%s:\n", scratch_label_name(l1, LABEL_ARRAY));
            
            struct expr *m = e->mid;
            while(m) {
                
                fprintf(out, "\t.quad %d\n", m->literal_value);
                m = m->next;
            }

            fprintf(out, ".text\n");
            e->reg = scratch_reg_alloc();
            fprintf(out, "\tleaq %s, %%%s\n", scratch_label_name(l1, LABEL_ARRAY), scratch_reg_name(e->reg));

            break;

        // only rt
        case EXPR_NOT:
            l1 = scratch_label_create(LABEL_JUMP);
            l2 = scratch_label_create(LABEL_JUMP);
            expr_codegen(e->right, out);
            e->reg = e->right->reg;
            fprintf(out, " # expr not\n");
            fprintf(out, "\tcmpq $0, %%%s\n", scratch_reg_name(e->right->reg));
            fprintf(out, "\tje %s\n", scratch_label_name(l1, LABEL_JUMP));
            fprintf(out, "\tmovq $0, %%%s\n", scratch_reg_name(e->right->reg));
            fprintf(out, "\tjmp %s\n", scratch_label_name(l2, LABEL_JUMP));
            fprintf(out, "%s:\n", scratch_label_name(l1, LABEL_JUMP));
            fprintf(out, "\tmovq $1, %%%s\n", scratch_reg_name(e->right->reg));
            fprintf(out, "%s:\n", scratch_label_name(l2, LABEL_JUMP));


            break;
        case EXPR_NEG:
            expr_codegen(e->right, out);
            e->reg = e->right->reg;
            fprintf(out, " # negating\n");
            fprintf(out, "\tnegq %%%s\n", scratch_reg_name(e->right->reg));
             break;

        // only lt
        case EXPR_INC:
             fprintf(out, "# generating code for ++\n");
             expr_codegen(e->left, out);
             e->reg = scratch_reg_alloc();
             fprintf(out, "\tmovq %%%s, %%%s\n", scratch_reg_name(e->left->reg), scratch_reg_name(e->reg));
             fprintf(out, "\tincq %%%s\n", scratch_reg_name(e->left->reg));
             fprintf(out, "\tmovq %%%s, %s\n", scratch_reg_name(e->left->reg), symbol_codegen(e->left->symbol));
             scratch_reg_free(e->left->reg); 
             break;
        case EXPR_DEC:
             fprintf(out, "# generating code for --\n");
             expr_codegen(e->left, out);
             e->reg = scratch_reg_alloc();
             fprintf(out, "\tmovq %%%s, %%%s\n", scratch_reg_name(e->left->reg), scratch_reg_name(e->reg));
             fprintf(out, "\tdecq %%%s\n", scratch_reg_name(e->left->reg));
             fprintf(out, "\tmovq %%%s, %s\n", scratch_reg_name(e->left->reg), symbol_codegen(e->left->symbol));
             scratch_reg_free(e->left->reg); 
             break;
 
        // special
        case EXPR_TERN:
            expr_codegen(e->left, out);
            int l1 = scratch_label_create(LABEL_JUMP);
            int l2 = scratch_label_create(LABEL_JUMP);
            e->reg = scratch_reg_alloc();

            fprintf(out, "# generating code for tern\n");
           
            fprintf(out, "\tcmp $0, %%%s\n", scratch_reg_name(e->left->reg));
            fprintf(out, "\tje %s\n", scratch_label_name(l1, LABEL_JUMP));
            scratch_reg_free(e->left->reg);
            
            // true
            expr_codegen(e->mid, out);
            fprintf(out, "\tmovq %%%s, %%%s\n", 
                    scratch_reg_name(e->mid->reg),
                    scratch_reg_name(e->reg));
            scratch_reg_free(e->mid->reg);
            fprintf(out, "\tjmp %s\n", scratch_label_name(l2, LABEL_JUMP));
            

            // false
            fprintf(out, "%s:\n", scratch_label_name(l1, LABEL_JUMP));
            expr_codegen(e->right, out);
            fprintf(out, "\tmovq %%%s, %%%s\n", 
                    scratch_reg_name(e->right->reg),
                    scratch_reg_name(e->reg));
            scratch_reg_free(e->right->reg);

            // cont
            fprintf(out, "%s:\n", scratch_label_name(l2, LABEL_JUMP));
            break;
        case EXPR_ASSIGN:
            if(e->left->kind == EXPR_IDENT) {
                fprintf(out, "# genterating code for ident =\n");
                // value
                expr_codegen(e->right, out);
                e->reg = e->right->reg;
                
                // address
                e->left->reg = scratch_reg_alloc();
                fprintf(out, "\tleaq %s, %%%s\n", 
                        symbol_codegen(e->left->symbol), 
                        scratch_reg_name(e->left->reg));
                fprintf(out, "# loading value\n");
                fprintf(out, "\tmovq %%%s, (%%%s)\n", 
                    scratch_reg_name(e->right->reg),
                    scratch_reg_name(e->left->reg));
                scratch_reg_free(e->left->reg);
            } else if (e->left->kind == EXPR_ARRAY) {
                fprintf(out, "# generating code for array =\n");

                // value to assign to
                expr_codegen(e->left->mid, out);
                e->left->reg = scratch_reg_alloc();

                // value to assign
                expr_codegen(e->right, out);
                e->reg = e->right->reg;

                // address
                fprintf(out, "\tleaq %s, %%%s\n", symbol_codegen(e->left->symbol), scratch_reg_name(e->left->reg));

                // index
                fprintf(out, "\tmovq %%%s, %%rax\n", scratch_reg_name(e->left->mid->reg));

                int ri = scratch_reg_alloc();
                fprintf(out, "\tmovq $8, %%%s\n", scratch_reg_name(ri));
                fprintf(out, "\timulq %%%s\n", scratch_reg_name(ri));

                // load back
                fprintf(out, "\taddq %%%s, %%rax\n", scratch_reg_name(e->left->reg));
                fprintf(out, "\tmovq %%%s, (%%rax)\n", scratch_reg_name(e->right->reg));
                
                // free
                scratch_reg_free(ri);
                scratch_reg_free(e->left->mid->reg);
                scratch_reg_free(e->left->reg);
                
            }
            break;

        // only integers
        case EXPR_ADD:
            fprintf(out, "# generating code for add\n");
            expr_codegen(e->left, out);
            expr_codegen(e->right, out);
            e->reg = e->right->reg;
            fprintf(out, "\taddq %%%s, %%%s\n",scratch_reg_name(e->left->reg), scratch_reg_name(e->right->reg));
            scratch_reg_free(e->left->reg);
            break;
        case EXPR_SUB:
            fprintf(out, "# generating code for sub\n");
            expr_codegen(e->left, out);
            expr_codegen(e->right, out);
            e->reg = e->left->reg;
            fprintf(out, "\tsubq %%%s, %%%s\n",scratch_reg_name(e->right->reg), scratch_reg_name(e->left->reg));
            scratch_reg_free(e->right->reg);           
            break;
        case EXPR_MUL:
            fprintf(out, "# generating code for mult\n");
            expr_codegen(e->left, out);
            expr_codegen(e->right, out);
            e->reg = e->right->reg;
            fprintf(out, "# doing the mult (rax)\n");
            fprintf(out, "\tmovq %%%s, %%rax\n", scratch_reg_name(e->right->reg));
            fprintf(out, "\timulq %%%s\n", scratch_reg_name(e->left->reg));
            fprintf(out, "\tmovq %%rax, %%%s\n", scratch_reg_name(e->right->reg));
            scratch_reg_free(e->left->reg);
            break;
        case EXPR_DIV:
            fprintf(out, "# generating code for div\n");
             expr_codegen(e->left, out);
            expr_codegen(e->right, out);
            e->reg = e->right->reg;
            fprintf(out, "# doing the div (rax)\n");
            fprintf(out, "\tmovq %%%s, %%rax\n", scratch_reg_name(e->left->reg));
            fprintf(out, "\tcqo\n");
            fprintf(out, "\tidivq %%%s\n", scratch_reg_name(e->right->reg));
            fprintf(out, "\tmovq %%rax, %%%s\n", scratch_reg_name(e->right->reg));
            scratch_reg_free(e->left->reg);
            break;
        case EXPR_MOD:
            fprintf(out, "# generating code for mod\n");
            expr_codegen(e->left, out);
            expr_codegen(e->right, out);
            e->reg = e->right->reg;
            fprintf(out, "# doing the mod (rax)\n");
            fprintf(out, "\tmovq %%%s, %%rax\n", scratch_reg_name(e->left->reg));
            fprintf(out, "\tcqo\n");
            fprintf(out, "\tidivq %%%s\n", scratch_reg_name(e->right->reg));
            fprintf(out, "\tmovq %%rdx, %%%s\n", scratch_reg_name(e->right->reg));
            scratch_reg_free(e->left->reg);
            break;
        case EXPR_EX:
            fprintf(out, "# generating code for pow\n");
            expr_codegen(e->left, out);
            expr_codegen(e->right, out);
            fprintf(out, "\tpushq %%r10\n");
            fprintf(out, "\tpushq %%r11\n");
            e->reg = e->right->reg;
            fprintf(out, "\tmovq %%%s, %%rdi\n", scratch_reg_name(e->left->reg));
            fprintf(out, "\tmovq %%%s, %%rsi\n", scratch_reg_name(e->right->reg));
            fprintf(out, "\tcall integer_power\n");
            fprintf(out, "\tpopq %%r11\n");
            fprintf(out, "\tpopq %%r10\n");
            fprintf(out, "\tmovq %%rax, %%%s\n", scratch_reg_name(e->reg));
            scratch_reg_free(e->left->reg);
            break;

        // logical
        case EXPR_OR:
            fprintf(out, "# generating code for or\n");
            expr_codegen(e->left, out);
            expr_codegen(e->right, out);
            e->reg = e->right->reg;
            fprintf(out, "# doing the or\n");
            fprintf(out, "\torq %%%s, %%%s\n", scratch_reg_name(e->left->reg), scratch_reg_name(e->right->reg));
            scratch_reg_free(e->left->reg);
            break;

        case EXPR_AND:
            fprintf(out, "# generating code for and\n");
            expr_codegen(e->left, out);
            expr_codegen(e->right, out);
            e->reg = e->right->reg;
            fprintf(out, "# doing the or\n");
            fprintf(out, "\tandq %%%s, %%%s\n", scratch_reg_name(e->left->reg), scratch_reg_name(e->right->reg));
            scratch_reg_free(e->left->reg);
            break;

        // compare
        case EXPR_LT:
        case EXPR_LTE:
        case EXPR_GT:
        case EXPR_GTE:
        case EXPR_EQ:
        case EXPR_NEQ:
            expr_cmp_codegen(e, out);
            break;
    }
 
    return;
}
