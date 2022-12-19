#include "decl.h"
#include "expr.h"
#include "stmt.h"
#include "error.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

int First_global = 1;
/***********************************************************************
 * Creates a decl struct                                               *
 ***********************************************************************/
struct decl * decl_create( char *name, struct type *type, struct expr *value, struct stmt *code, struct decl *next ) {
    struct decl *d = malloc(sizeof(struct decl));
    if (!d)
        return NULL;

    d->name = name;
    d->type = type;
    d->value = value;
    d->code = code;
    d->next = next;

    d->num_params = 0;
    d->num_locals = 0;
    d->num_total = 0;
    d->found_return = 0;

    return d;
}

/***********************************************************************
 * Prints decl structures                                              *
 ***********************************************************************/
void decl_print( struct decl *d, int indent ) {
    if(!d) return;
    indent_print(indent);
    printf("%s: ", d->name);
    type_print(d->type);
    if(d->value) {
        printf(" = ");
        expr_print(d->value);
        puts(";");
    } else if (d->code) {
        printf(" = ");
        stmt_print(d->code, indent);
    } else {
        puts(";");
    }
    decl_print(d->next, indent);
}

/***********************************************************************
 * Prints a certain indent amount                                      *
 ***********************************************************************/
void indent_print(int indent) {
  for (int i = 0; i < indent; i++) {
        printf("    ");
  }
}

/***********************************************************************
 * Resolve a decl                                                      *
 ***********************************************************************/
int decl_resolve(struct scope *s, struct decl *d) {
    int status = 0;
    if(!d) return status;

    // create symbol
    symbol_t kind = s->level ? SYMBOL_LOCAL : SYMBOL_GLOBAL;
    struct symbol *n = symbol_create(kind, d->type, d->name); 
 
    // resolve expr
    status += expr_resolve(s, d->value);

    // bind it
    if(!scope_bind(s, d->name, n))
        status = 1;

    // update values
    n->which = s->which;
    d->symbol = n;

    // resolve code
    if (d->code) {
        s = scope_enter(s);
        param_resolve(s, d->type->params);
        //printf("params: %d\n", s->which);
        d->num_params = s->which;
        //s->which = 0;
        status += stmt_resolve(s, d->code->body);
        d->num_locals = s->which - d->num_params;
        d->num_total = d->num_params + d->num_locals;
        //printf("locals: %d\n", s->which);
        s = scope_exit(s);
    }

    // check nexts and return status
    status += decl_resolve(s, d->next);
    return status;
}

/***********************************************************************
 * check base subtype of an array                                      *
 ***********************************************************************/
void decl_array_typecheck(struct decl *d) {
    struct type *t = d->symbol->type;

    // get to base type
    while(t->subtype)
        t = t->subtype;

    // make sure not void or function
    if(t->kind == TYPE_VOID 
            || t->kind == TYPE_FUNCTION)
        error_print(ERR_EXPR_BAD_ARR_TYPE, d, 0);
}


/***********************************************************************
 * check for autos in array decls and update type                      *
 ***********************************************************************/
void decl_array_auto_check(struct decl *d, struct type *lit) {
    struct type *a = d->symbol->type;
    struct type *b = lit;

    if(!d || !lit) return;

    // check each subtype, and update if needed
    while(a->subtype && b->subtype) {
        if(a->subtype->kind == TYPE_AUTO) {
            a->subtype = type_copy(b->subtype);
            printf("notice: type of %s is now ", d->name);
            type_print(d->symbol->type);
            printf("\n");
            return;
        }
        a = a->subtype;
        b = b->subtype;
    }
}


/***********************************************************************
 * Typecheck a decl                                                    *
 ***********************************************************************/
void decl_typecheck( struct decl *d ) {
    if(!d) return;

    // cant have non-subtype of void
    if(d->type->kind == TYPE_VOID) {
        error_print(ERR_VOID, d, 0);
    }

    // typecheck the value
    if(d->value) {
        if(d->symbol->kind == SYMBOL_GLOBAL 
                && d->value->kind != EXPR_INT
                && d->value->kind != EXPR_CHAR
                && d->value->kind != EXPR_STR
                && d->value->kind != EXPR_BOOL
                && d->value->kind != EXPR_ARRAY_LIT) 
            error_print(ERR_GLOBAL_CONST, d, 0);

        // if an array literal, check for global init and convert
        if(d->value->kind == EXPR_ARRAY_LIT) {
            // only array decls in global
            if(d->symbol->kind != SYMBOL_GLOBAL)
                error_print(ERR_EXPR_LIT_GLO, d, 0);

            // convert literal
            d->symbol->type->expr = expr_copy(d->type->expr);
            struct type *lit = type_literal_convert(d->value);

            // check for auto
            decl_array_auto_check(d, lit);

            // error check
            d->value->error = type_copy(lit);
            if(!type_equals(d->symbol->type, lit)) 
                error_print(ERR_DECL_VAL, d, 0);
            
        // else just check
        } else {
            // check decl value mismatch
            struct type *t = expr_typecheck(d->value);

            // check auto
            if(d->symbol->type->kind == TYPE_AUTO) {
                if(t->kind == TYPE_AUTO)
                    error_print(ERR_AUTO, d, 0);

                d->symbol->type = type_copy(t);
                printf("notice: type of %s is now ", d->name);
                type_print(t); printf("\n");
            }

            // error check
            d->value->error = type_copy(t);

            // if auto, update
            if(d->symbol->type->kind == TYPE_AUTO) {
                if(t->kind == TYPE_AUTO)
                    error_print(ERR_AUTO, d, 0);
                d->symbol->type = type_copy(t);
                printf("notice: type of %s is now ", d->name);
                type_print(d->symbol->type);
                printf("\n");
            }

            if(t->kind !=  d->symbol->type->kind)
                error_print(ERR_DECL_VAL, d, 0);

        }
 
    } 

    // check base type of an array
    if(d->symbol->type->kind == TYPE_ARRAY)
        decl_array_typecheck(d);

    // check params for auto
    if(param_typecheck(d->symbol->type->params))
        error_print(ERR_AUTO_PARAM, d, 0);

    // check code
    if(d->code) {
        stmt_typecheck(d->code, d);

        // change return type of function if auto
        if(d->symbol->type->subtype->kind == TYPE_AUTO) {
            d->symbol->type->subtype = type_create(TYPE_VOID, 0, 0);
            printf("notice: return type of function %s is now ", d->name);
            type_print(d->symbol->type->subtype);
            printf("\n");
        }
    }

    // check for a return if non-void
    if(d->symbol->type->kind == TYPE_FUNCTION 
            && d->symbol->type->subtype->kind != TYPE_VOID 
            && d->found_return == 0 
            && d->code
            && strcmp(d->name, "main"))
        error_print(ERR_NO_RETURN, d, 0);
 
    // check next
    decl_typecheck(d->next);
    return;
}

/***********************************************************************
 * helper to print out code for decl                                   *
 ***********************************************************************/
void decl_codegen_array(struct decl *d, FILE *out, int g) {
    if(g) fprintf(out, "%s:\n", d->name);
    struct expr *e = 0;
    if(d->value)
        e = d->value->mid;
            
    for (int i = 0; i < d->type->expr->literal_value; i++) {
        fprintf(out, "\t.quad %d\n", d->value ? e->literal_value : 0);
        if(e)
            e = e->next;
    }
}

/***********************************************************************
 * helper to print out code prologue                                    *
 ***********************************************************************/
void decl_prologue(struct decl *d, FILE *out) {
    extern char *Arg_regs[6];
    extern bool Registers[7];

    fprintf(out, "# Start of function prologue\n");
    // setup framei
    fprintf(out, "\tpushq %%rbp\n");
    fprintf(out, "\tmovq %%rsp, %%rbp\n");

    // save params
    for(int i = 0; i < d->num_params; i++) {
        fprintf(out, "\tpushq %%%s\n", Arg_regs[i]);
    }

    // allocate locals
    if(d->num_locals) fprintf(out, "\tsubq $%d, %%rsp\n", (d->num_locals*8));

    // save registers
    if(strcmp(d->name, "man")) {
        fprintf(out, "\tpushq %%rbx\n");
        fprintf(out, "\tpushq %%r12\n");
        fprintf(out, "\tpushq %%r13\n");
        fprintf(out, "\tpushq %%r14\n");
        fprintf(out, "\tpushq %%r15\n");
    }
    fprintf(out, "# End of functiop prologue\n");
    
}

/***********************************************************************
 * helper to print out code epilogue                                    *
 ***********************************************************************/
void decl_epilogue(struct decl *d, FILE *out) {
    
    fprintf(out, "# Start of function epilogue\n");
    // restore registers
    fprintf(out, ".%s_epilogue:\n", d->name);
    
    if(strcmp(d->name, "man")) {
        fprintf(out, "\tpopq %%r15\n");
        fprintf(out, "\tpopq %%r14\n");
        fprintf(out, "\tpopq %%r13\n");
        fprintf(out, "\tpopq %%r12\n");
        fprintf(out, "\tpopq %%rbx\n");
    }

    // deallocate locals

    // restore frame
    fprintf(out, "\tmovq %%rbp, %%rsp\n");
    fprintf(out, "\tpopq %%rbp\n");
    fprintf(out, "\tret\n");
    fprintf(out, "# End of function epilogue\n");
}

/***********************************************************************
 * Typecheck a decl                                                    *
 ***********************************************************************/
void decl_codegen(struct decl *d, FILE *out) {
    if(!d) 
        return;
    
    // Global decls
    if(d->symbol->kind == SYMBOL_GLOBAL) {
        
        switch (d->symbol->type->kind) {
            case TYPE_FUNCTION: 
                if(d->code) {
                    // first to make declaring a bunch in a row
                    First_global = 1;
                    fprintf(out, ".text\n");
                    fprintf(out, ".global %s\n", d->name);
                    fprintf(out, "%s:\n", d->name);
                    decl_prologue(d, out);
                    stmt_codegen(d->code, out);
                    decl_epilogue(d, out);
                }
                break;
        
            case TYPE_INTEGER:
            case TYPE_BOOLEAN:
                if(First_global) {
                    fprintf(out, ".data\n");
                    First_global = 0;
                }
                fprintf(out, ".global %s\n", d->name);
                fprintf(out, "%s:\n\t.quad %d\n", d->name, d->value ? d->value->literal_value : 0);
                break;
            case TYPE_STRING:
                if(First_global) {
                    fprintf(out, ".data\n");
                    First_global = 0;
                }
                fprintf(out, ".global %s\n", d->name);
                if(d->value) {
                    int l = scratch_label_create(LABEL_STRING);
                    fprintf(out, "%s:\n\t.quad %s\n", d->name, scratch_label_name(l, LABEL_STRING));
                    fprintf(out, "%s:\n\t.string %s\n", scratch_label_name(l, LABEL_STRING), d->value->string_literal);
                }
                break;
            case TYPE_CHARACTER: 
                if(First_global) {
                    fprintf(out, ".data\n");
                    First_global = 0;
                }
                fprintf(out, ".global %s\n", d->name);
                fprintf(out, "%s:\n\t.quad %d\n", d->name, d->value ? *(d->value->string_literal+1) : 'a');
                break;
            case TYPE_ARRAY:
                if(First_global) {
                    fprintf(out, ".data\n");
                    First_global = 0;
                }
                fprintf(out, ".global %s\n", d->name);
                decl_codegen_array(d, out, 1);
                break;
            default:
                break;
        }
    

    // Local decls
    } else {
        int l1, r1;
        switch (d->symbol->type->kind) {
            case TYPE_CHARACTER:
            case TYPE_STRING:
            case TYPE_BOOLEAN:
            case TYPE_INTEGER:
                if(d->value) {
                    fprintf(out, "# generating code for value of decl %s\n", d->name);
                    expr_codegen(d->value, out);
                    fprintf(out, "# generating code for decl %s\n", d->name);
                    fprintf(out, "\tmovq %%%s, %s\n", 
                            scratch_reg_name(d->value->reg), 
                            symbol_codegen(d->symbol));
                    scratch_reg_free(d->value->reg);

                }
                break;
            case TYPE_ARRAY:
                // TODO for fun later
                break; 
            default:
                break;
        }
    }

    decl_codegen(d->next, out);
}
