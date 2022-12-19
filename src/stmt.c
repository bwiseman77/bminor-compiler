#include "stmt.h"
#include "decl.h"
#include "error.h"
#include <stdlib.h>
#include <stdio.h>

/***********************************************************************
 * Creates a stmt struct                                               *
 ***********************************************************************/
struct stmt * stmt_create( stmt_t kind, struct decl *decl, struct expr *init_expr, struct expr *expr, struct expr *next_expr, struct stmt *body, struct stmt *else_body, struct stmt *next ) {
    struct stmt *s = malloc(sizeof(struct stmt));
    if(!s)
        return NULL;

    s->kind = kind;
    s->decl = decl;
    s->init_expr = init_expr;
    s->expr = expr;
    s->next_expr = next_expr;
    s->body = body;
    s->else_body = else_body;
    s->else_if = 0;
    s->fi = 0;
    s->no_indent = 0;
    s->is_func = 0;
    return s;
}

/***********************************************************************
 * Figures out which stmt to print                                     *
 ***********************************************************************/
void stmt_print(struct stmt *s, int indent) {
    if(!s) return;
    switch (s->kind) {
        case STMT_DECL:
            stmt_decl_print(s, indent);
            break;
        case STMT_EXPR:
            stmt_expr_print(s, indent);
            break;
        case STMT_IF_ELSE:
            stmt_if_else_print(s, indent);
            break;
        case STMT_FOR:
            stmt_for_print(s, indent);
            break;
        case STMT_PRINT:
             stmt_print_print(s, indent);
             break;
        case STMT_RETURN:
             stmt_return_print(s, indent);
             break;
        case STMT_BLOCK:
            stmt_block_print(s, indent);
            break;
        case STMT_WHILE:
            stmt_while_print(s, indent);
            break;
    }
}

/***********************************************************************
 * Prints a decl stmt                                                  *
 ***********************************************************************/
void stmt_decl_print(struct stmt *s, int indent) {
    decl_print(s->decl, indent);
    stmt_print(s->next, indent);
}

/***********************************************************************
 * Prints a expr stmt                                                  *
 ***********************************************************************/
void stmt_expr_print(struct stmt *s, int indent) {
        indent_print(indent);
        expr_print(s->expr);
        printf(";\n");
        stmt_print(s->next, indent);    
}

/***********************************************************************
 * Prints a if_else stmt                                               *
 ***********************************************************************/
void stmt_if_else_print(struct stmt *s, int indent) {

    // only print indent if not after an else
    if(!s->else_if) indent_print(indent);

    // print if
    printf("if (");
    expr_print(s->expr);
    printf(") ");

    // print the body
    if(s->body) {
        if(s->body->kind != STMT_BLOCK) {
            printf("\n");
            stmt_print(s->body, indent);
        } else {
            stmt_print(s->body, indent);
        }
    } 

    // if part of an else if
    if(s->else_body) {

        indent_print(indent);

        printf("else ");

        // some extra logic for indenting based on stmt block vs reg stmt
        if(s->else_body) {
            if(s->else_body->kind != STMT_BLOCK) {
                if(s->else_body->kind != STMT_IF_ELSE) {
                    printf("\n");
                    stmt_print(s->else_body, indent+1);
                } else {
                    stmt_print(s->else_body, indent);
                }
            } else {
                stmt_print(s->else_body, indent);
            }
        }
    }

    // print next stmt
    stmt_print(s->next, indent);
}

/***********************************************************************
 * Prints a for stmt                                                  *
 ***********************************************************************/
void stmt_for_print(struct stmt *s, int indent) {
    
    // print for stmt
    indent_print(indent);
    printf("for("); 
    expr_print(s->init_expr);
    printf(";");
    expr_print(s->expr);
    printf(";");
    expr_print(s->next_expr);
    printf(") ");
    
    // extra logic to onl
    if(s->body) {
        if(s->body->kind != STMT_BLOCK) {
            printf("\n");
      
            stmt_print(s->body, indent+1);
         } else {
            stmt_print(s->body, indent);
         }
    }
    
    stmt_print(s->next, indent);
}

/***********************************************************************
 * Prints a block stmt                                                 *
 ***********************************************************************/
void stmt_block_print(struct stmt *s, int indent) {
    if(s->else_if || s->fi || s->no_indent) ; 
    else
        indent_print(indent);

    printf("{\n");
    stmt_print(s->body, indent+1);
    indent_print(indent);
    printf("}\n");
    stmt_print(s->next, indent);
}

/***********************************************************************
 * Prints a return stmt                                                *
 ***********************************************************************/
void stmt_return_print(struct stmt *s, int indent) {
    indent_print(indent);
    printf("return ");
    expr_print(s->expr);
    printf(";\n");
    stmt_print(s->next, indent);
}

/***********************************************************************
 * Prints a print stmt                                                 *
 ***********************************************************************/
void stmt_print_print(struct stmt *s, int indent) {
    indent_print(indent);
    printf("print ");
    expr_list_print(s->expr);
    printf(";\n");
    stmt_print(s->next, indent);
}

/***********************************************************************
 * Prints a while stmt                                                 *
 ***********************************************************************/
void stmt_while_print(struct stmt *s, int indent) {
    indent_print(indent);
    printf("while (");
    expr_print(s->expr);
    printf(") ");
    if(s->body) {
        if(s->body->kind != STMT_BLOCK) {
            printf("\n");
            stmt_print(s->body, indent+1);
        } else {
            stmt_print(s->body, indent);
        }
    } 
    stmt_print(s->next, indent);
}

/***********************************************************************
 * Resolve a stmt                                                      *
 ***********************************************************************/
int stmt_resolve(struct scope *c, struct stmt *s) {
    int status = 0;
    if(!s) return status;
    int temp_which;
    switch (s->kind) {
        case STMT_DECL:
            status += decl_resolve(c, s->decl);
            break;
        case STMT_EXPR:
            status += expr_resolve(c, s->expr);
            break;
        case STMT_BLOCK:
            temp_which = c->which;
            c = scope_enter(c);
            if(!s->is_func) c->which = temp_which;
            status += stmt_resolve(c, s->body);
            temp_which = c->which;
            c = scope_exit(c);
            c->which = temp_which;
            break;
        case STMT_IF_ELSE:
            status += expr_resolve(c, s->expr);
            status += stmt_resolve(c, s->body);
            status += stmt_resolve(c, s->else_body);
            break;
        case STMT_FOR:
            status += expr_resolve(c, s->init_expr);
            status += expr_resolve(c, s->expr);
            status += expr_resolve(c, s->next_expr);
            status += stmt_resolve(c, s->body);
            break;
        case STMT_PRINT:
            status += expr_resolve(c, s->expr);
            break;
        case STMT_RETURN:
            status += expr_resolve(c, s->expr);
            break;
        case STMT_WHILE:
            status += expr_resolve(c, s->expr);
            status += stmt_resolve(c, s->body);
            break;
    }

    status += stmt_resolve(c, s->next);
    return status;
}

/***********************************************************************
 * typecheck a stmt                                                    *
 ***********************************************************************/
struct type * stmt_typecheck(struct stmt *s, struct decl *return_type) {
    if(!s) return 0;
    
    struct type *result;
    switch(s->kind) {
        case STMT_BLOCK:
            stmt_typecheck(s->body, return_type); 
            break;
        case STMT_EXPR:
            expr_typecheck(s->expr);
            break;    
        case STMT_RETURN:
            // first typecheck expr
            result = expr_typecheck(s->expr);

            // check if we need to update the type if auto
            if(return_type->symbol->type->subtype->kind == TYPE_AUTO) {

                if(result && result->kind == TYPE_AUTO)
                    error_print(ERR_AUTO, 0, 0);//
                    
                // if expr of return is null, means it is a void. so if null, create type of null, else copy the result
                return_type->symbol->type->subtype = (result != NULL) ? type_copy(result) : type_create(TYPE_VOID,0,0);
                result->kind = return_type->symbol->type->subtype->kind;
                printf("notice: return type of function %s is now ", return_type->name);
                type_print(return_type->symbol->type->subtype);
                printf("\n");
            }
  
            // check for mismatched return types
            if(!(result == NULL && return_type->symbol->type->subtype->kind == TYPE_VOID) 
                     && (!result || result->kind != return_type->symbol->type->subtype->kind)) {
  
                error_print(ERR_FUNC_TYPE, return_type, result);
            }
            // mark as finding a return
            s->name = return_type->name;
            return_type->found_return = 1;
            break;
        case STMT_DECL:

            decl_typecheck(s->decl);
            break;
        case STMT_IF_ELSE:

            result = expr_typecheck(s->expr);
            s->expr->loop_expr = 1;
            // check that condition is a boolean
            if(result->kind != TYPE_BOOLEAN)
                error_print(ERR_IF_BOOL, s->expr, result);//
            stmt_typecheck(s->body, return_type);
            stmt_typecheck(s->else_body, return_type);
            break;
        case STMT_WHILE:
            result = expr_typecheck(s->expr);
            s->expr->loop_expr = 1;
            // check condition is a boolean
            if(result->kind != TYPE_BOOLEAN)
                error_print(ERR_WHILE_BOOL, s->expr, result);//
            stmt_typecheck(s->body, return_type);
            break;
        case STMT_PRINT:
            expr_typecheck(s->expr);
            break;
        case STMT_FOR:
            result = expr_typecheck(s->init_expr);
            result = expr_typecheck(s->expr);
            if(s->expr && result->kind != TYPE_BOOLEAN)
                error_print(ERR_WHILE_BOOL, s->expr, result);
            if(s->expr) s->expr->loop_expr = 1;
            result = expr_typecheck(s->next_expr);
            result = stmt_typecheck(s->body, return_type);
            break;
        default:
            break;
    }

    return stmt_typecheck(s->next, return_type);
}

/***********************************************************************
 * helper to codegen a print stmt                                      *
 ***********************************************************************/
void stmt_print_codegen(struct stmt *s, FILE *out) {
    struct expr *curr = s->expr;
    struct type *t;
    fprintf(out, "# printing exprs\n");
    while(curr) {
        t = expr_typecheck(curr);
        fprintf(out, "\tpushq %%r10\n");
        fprintf(out, "\tpushq %%r11\n");
        expr_codegen(curr, out);
        fprintf(out, "\tmovq %%%s, %%rdi\n", scratch_reg_name(curr->reg));
        switch (t->kind) {
            case TYPE_INTEGER:                
                fprintf(out, "\tcall print_integer\n");
                break;
            case TYPE_STRING:
                fprintf(out, "\tcall print_string\n");
                break;
            case TYPE_CHARACTER:
                fprintf(out, "\tcall print_character\n");
                break;
            case TYPE_BOOLEAN:
                fprintf(out, "\tcall print_boolean\n");
                break;
        }
        fprintf(out, "\tpopq %%r11\n");
        fprintf(out, "\tpopq %%r10\n");

        scratch_reg_free(curr->reg);
        curr = curr->next;
    }
}

/***********************************************************************
 * helper to codegen a while stmt                                      *
 ***********************************************************************/
void stmt_while_codegen(struct stmt *s, FILE *out) {
    fprintf(out, "#generating code for while loop\n");

    // top and done labels
    int top = scratch_label_create(LABEL_JUMP);
    int done = scratch_label_create(LABEL_JUMP);
    fprintf(out, "%s:\n", scratch_label_name(top, LABEL_JUMP));

    // expr
    fprintf(out, "# condition expr\n");
    expr_codegen(s->expr, out);
    fprintf(out, "\tcmpq $0, %%%s\n", scratch_reg_name(s->expr->reg));
    fprintf(out, "\tje %s\n", scratch_label_name(done, LABEL_JUMP));
    
    // body
    fprintf(out, "# while body\n");
    stmt_codegen(s->body, out);

    // jmp top
    fprintf(out, "\tjmp %s\n", scratch_label_name(top, LABEL_JUMP));

    // done label
    fprintf(out, "%s:\n", scratch_label_name(done, LABEL_JUMP));
    
    scratch_reg_free(s->expr->reg);
}

/***********************************************************************
 * helper to codegen a for stmt                                        *
 ***********************************************************************/
void stmt_for_codegen(struct stmt *s, FILE *out) {
    fprintf(out, "# generating code for loop\n");

    // init expr
    fprintf(out, "# init expr\n");
    expr_codegen(s->init_expr, out);


    // top and done label
    int top = scratch_label_create(LABEL_JUMP);
    int done = scratch_label_create(LABEL_JUMP);
    fprintf(out, "%s:\n", scratch_label_name(top, LABEL_JUMP));

    // expr
    fprintf(out, "# condition expr\n");
    expr_codegen(s->expr, out);
    if(s->expr) fprintf(out, "\tcmpq $0, %%%s\n", scratch_reg_name(s->expr->reg));
    if(s->expr) fprintf(out, "\tje %s\n", scratch_label_name(done, LABEL_JUMP));
    

    // body
    fprintf(out, "# for body\n");
    stmt_codegen(s->body, out);

    // next expr
    fprintf(out, "# next expr\n");
    expr_codegen(s->next_expr, out);

    // jmp top
    fprintf(out, "\tjmp %s\n", scratch_label_name(top, LABEL_JUMP));

    // done label
    fprintf(out, "%s:\n", scratch_label_name(done, LABEL_JUMP));

    if(s->init_expr) scratch_reg_free(s->init_expr->reg);
    if(s->expr) scratch_reg_free(s->expr->reg);
    if(s->next_expr) scratch_reg_free(s->next_expr->reg);
}    

/***********************************************************************
 * helper to codegen a if stmt                                         *
 ***********************************************************************/
void stmt_if_codegen(struct stmt *s, FILE *out) {
    fprintf(out, "# generating code for if stmt\n");

    // l1: false; l2: done
    int l1 = scratch_label_create(LABEL_JUMP);
    int l2 = scratch_label_create(LABEL_JUMP);;
    
    // expr cond
    fprintf(out, "# condition expr\n");
    expr_codegen(s->expr, out);

    
    fprintf(out, "\tcmpq $0, %%%s\n", scratch_reg_name(s->expr->reg));
    fprintf(out, "\tje %s\n", scratch_label_name(l1, LABEL_JUMP));
    
    // if body
    stmt_codegen(s->body, out);
    fprintf(out, "\tjmp %s\n", scratch_label_name(l2, LABEL_JUMP));

    // false label
    fprintf(out, "%s:\n", scratch_label_name(l1, LABEL_JUMP));

    // else body
    stmt_codegen(s->else_body, out);

    // done label
    fprintf(out, "%s:\n", scratch_label_name(l2, LABEL_JUMP));

    scratch_reg_free(s->expr->reg);
}

/***********************************************************************
 * codegen a stmt                                                      *
 ***********************************************************************/
void stmt_codegen(struct stmt *s, FILE *out) {
    if(!s) return;
    
     switch(s->kind) {
        case STMT_BLOCK:
            stmt_codegen(s->body, out);
            break;
        case STMT_EXPR:
            s->expr->func_value = 1;
            expr_codegen(s->expr, out);
            scratch_reg_free(s->expr->reg);
            break;    
        case STMT_RETURN:
            fprintf(out, "# generating code for return\n");
            expr_codegen(s->expr, out);
            fprintf(out, "# generating code for return\n");
            if(s->expr) fprintf(out, "\tmovq %%%s, %%rax\n", scratch_reg_name(s->expr->reg));
            fprintf(out, "\tjmp .%s_epilogue\n", s->name);
            if(s->expr) scratch_reg_free(s->expr->reg);
            break;
        case STMT_DECL:
             decl_codegen(s->decl, out);              
             break;
        case STMT_IF_ELSE:
             stmt_if_codegen(s, out);
            break;
        case STMT_WHILE:
            stmt_while_codegen(s, out);
            break;
        case STMT_PRINT:
            stmt_print_codegen(s, out);
            break;
        case STMT_FOR:
            stmt_for_codegen(s, out);
            break;
        default:
            break;
    }

    stmt_codegen(s->next, out);
}
