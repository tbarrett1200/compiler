#include <stdio.h>
#include <stdbool.h>
#include <debug.h>
#include <string.h>
#include <ast.h>
#include <scanner.h>
#include <parser.h>
#include <string.h>
#include <container/vector.h>
#include <container/hash.h>

//the output file
FILE *dst = NULL;
//the current indentation level
int indent = 0;
//the number of variables declared in current scope
int varnum = 0;

//a struct which specifies both the type and number of an IR Value
typedef struct Value {
    int type;
    int val;
} Value;

#define VALUE_ERROR -1
#define VALUE_VARIABLE 0
#define VALUE_NUMBER 1

void auto_indent(void);
Value emit_factor(Factor *factor,SymbolTable *sym);
Value emit_expression(Expression *e,SymbolTable *sym);
void emit_statement(Statement *s,SymbolTable*);
void emit_declaration(Declaration *d,SymbolTable*);
void emit_block(Block *b);


void auto_indent(void){
  for (int i=0;i<indent; i++)
  {
    fprintf(dst,"\t");
  }
}
void getValueString(Value v,char str[16]) {
    if (v.type == VALUE_NUMBER) snprintf(str,16,"%d",v.val);
    else if (v.type == VALUE_VARIABLE) snprintf(str,16,"%%%d",v.val);
}
void getOpString(int op, char str[16]) {
    switch (op)
    {
        case ADD: snprintf(str,16,"add");
        break;
        case SUBTRACT: snprintf(str,16, "sub");
        break;
        case MULTIPLY: snprintf(str,16, "mul");
        break;
        case DIVIDE: snprintf(str,16, "sdiv");
        break;
        case MODULUS: snprintf(str,16, "srem");
        break;
    }
}

Value emit_factor(Factor *factor, SymbolTable *sym) {
    if (factor == NULL || sym == NULL);
    else if (factor->type == FACTOR_INTEGER) {
        return (Value){VALUE_NUMBER,factor->integer};
    }
    else if (factor->type == FACTOR_IDENTIFIER) {
        Identifier *d = symbol_table_get(sym,factor->identifier);
        if (d==NULL);
        else if (d->kind == IDENTIFIER_FUNCTION) {
            auto_indent();
            fprintf(dst,"%%%d = call i32 @%s()\n",varnum, factor->identifier);
            varnum++;
            return (Value){VALUE_VARIABLE,varnum-1};
        } else if (d->kind == IDENTIFIER_VARIABLE) {
            if (d->global) {
                auto_indent();
                fprintf(dst,"%%%d = load i32, i32* @%s\n",varnum, factor->identifier);
                varnum++;
                return (Value){VALUE_VARIABLE,varnum-1};
            } else {
                auto_indent();
                fprintf(dst,"%%%d = load i32, i32* %%%d\n",varnum, d->varnum);
                varnum++;
                return (Value){VALUE_VARIABLE,varnum-1};
            }
        }
    }
    else if (factor->type == FACTOR_EXPRESSION) {
        return emit_expression(factor->expression,sym);
    }
    else puts("error: unknown factor type in syntax tree");
    return (Value){VALUE_ERROR,-1};
}

Value emit_expression(Expression *e, SymbolTable *sym){
    if (e->type == EXPRESSION_BINARY) {
        if (e->op == EQUALS) {
            Value rval = emit_expression(e->r,sym);
            char right[16]={0};
            getValueString(rval,right);
            if (e->l->type != EXPRESSION_VALUE) {
                puts("error: can not assign");
            } else {
                char *name = e->l->value->identifier;
                Identifier *idt = symbol_table_get(sym,name);
                if (idt->global) {
                    auto_indent();
                    fprintf(dst, "store i32 %s, i32* @%s\n", right, name);
                } else {
                    auto_indent();
                    fprintf(dst, "store i32 %s, i32* %%%d\n", right, idt->varnum);
                }
            }
        } else {
            Value lval = emit_expression(e->l,sym);
            Value rval = emit_expression(e->r,sym);
            char op[16]={0};
            char left[16]={0};
            char right[16]={0};
            getOpString(e->op,op);
            getValueString(lval,left);
            getValueString(rval,right);
            auto_indent();
            fprintf(dst, "%%%d = %s i32 %s, %s\n", varnum, op,left, right);
            varnum++;
            return (Value){VALUE_VARIABLE,varnum-1};
        }
    }
    else if (e->type == EXPRESSION_UNARY) {
        Operator *op = operatorInfo(e->op,OPERATOR_PREFIX|OPERATOR_POSTFIX);
        if (op->token == SUBTRACT) {
            Value val = emit_expression(e->r,sym);
            char str[16]={0};
            getValueString(val,str);
            auto_indent();
            fprintf(dst, "%%%d = sub i32 0, %s\n", varnum, str);
            varnum++;
            return (Value){VALUE_VARIABLE,varnum-1};
        }
    }
    else if (e->type == EXPRESSION_VALUE) {
        return emit_factor(e->value, sym);
    }
    else puts("error: unknown expression type");
    return (Value){VALUE_ERROR,-1};
}

void emit_statement(Statement *s, SymbolTable *sym) {
    if (s == NULL || sym == NULL);
    else if (s->type == STATEMENT_EXPRESSION) {
        emit_expression(s->expression,sym);
    }
    else if (s->type == STATEMENT_BLOCK) {
      emit_block(s->block);
    }
    else if (s->type == STATEMENT_DECLARATION) {
      emit_declaration(s->declaration, sym);
    }
    else if (s->type == STATEMENT_RETURN) {
        Value val = emit_expression(s->expression, sym);
        char str[16]={0};
        getValueString(val,str);
        auto_indent();
        fprintf(dst,"ret i32 %s\n", str);
    }
    else puts("error: unknown statement type");
}

void emit_block(Block *b) {
    if (b != NULL) {
        for (int i=0; i<vector_size(b->statements); i++) {
            Statement *s = vector_get(b->statements,i);
            if (s == NULL) puts("error: null statement in block");
            emit_statement(s,b->symbols);
        }
    }
}

void emit_declaration(Declaration *d, SymbolTable *sym) {
    if (d == NULL || sym == NULL);
    else if (d->kind == DECLARATION_VARIABLE) {
        Identifier *var = symbol_table_get(sym,d->name);
        if (sym->parent == NULL) {
            var->global = 1;
            auto_indent();
            fprintf(dst, "@%s = global i32 0\n",d->name);
        } else {
            var->varnum = varnum++;
            auto_indent();
            fprintf(dst,"%%%d = alloca i32\n", var->varnum);
            auto_indent();
            fprintf(dst, "store i32 0, i32* %%%d\n", var->varnum);
        }
    }
    else if (d->kind == DECLARATION_ASSIGNMENT) {
        Identifier *var = symbol_table_get(sym,d->name);
        if (sym->parent == NULL) {
            if (d->value->type == EXPRESSION_VALUE && d->value->value->type == FACTOR_INTEGER)
            {
                var->global = 1;
                auto_indent();
                fprintf(dst, "@%s = global i32 %d",d->name, d->value->value->integer);
            }
            else printf("error: cannot assign global variable to expression");
        } else {
            Value expr = emit_expression(d->value,sym);
            var->varnum = varnum++;
            char loc[16] = {0};
            getValueString(expr,loc);
            auto_indent();
            fprintf(dst,"%%%d = alloca i32\n", var->varnum);
            auto_indent();
            fprintf(dst, "store i32 %s, i32* %%%d\n", loc, var->varnum);
        }
    }
    else if (d->kind == DECLARATION_FUNCTION) {
        Identifier *func = symbol_table_get(sym,d->name);
        varnum = d->block->symbols->varnum;
        auto_indent();
        fprintf(dst,"\ndefine i32 @%s () {\nentry:\n",d->name);
        indent++;
        emit_block(d->block);
        indent--;
        auto_indent();
        fprintf(dst,"}\n");

    }
}

void emit_program(FILE *f, Program *p){
    dst = f;
    fputs("target triple = \"x86_64-apple-macosx10.12.0\"\n",dst);
    if (p == NULL) puts("error: null program found in syntax tree");
    else {
        for (int i=0; i<vector_size(p->declarations); i++) {
            Declaration *d = vector_get(p->declarations,i);
            if (d == NULL) puts("error: null declaration in block");
            emit_declaration(d,p->globalSymbols);
        }
    }
}
