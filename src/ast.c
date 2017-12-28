#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <container/vector.h>
#include <container/hash.h>
#include <ast.h>
#include <symbol.h>

Factor* factor_init_with_integer(int i) {
    Factor *factor = malloc(sizeof(Factor));
    *factor = (Factor){FACTOR_INTEGER,i,NULL,NULL};
    return factor;
}
Factor* factor_init_with_identifier(char *c) {
    if (c == NULL) return NULL;
    Factor *factor = malloc(sizeof(Factor));
    *factor = (Factor){FACTOR_IDENTIFIER, 0, c, NULL};
    return factor;
}
Factor* factor_init_with_expression(Expression *e) {
    if (e == NULL) return NULL;
    Factor *factor = malloc(sizeof(Factor));
    *factor = (Factor){FACTOR_EXPRESSION,0,NULL, e};
    return factor;
}
void factor_destroy(Factor *f) {
    puts("destroying factor");
    if (f == NULL) return;
    else if (f->type == FACTOR_INTEGER);
    else if (f->type == FACTOR_IDENTIFIER) free(f->identifier);
    else if (f->type == FACTOR_EXPRESSION) expression_destroy(f->expression);
    else puts("error: unknown factor type in syntax tree");
    free(f);
}

Expression* expression_init_with_value(Factor *v) {
    if (v == NULL) return NULL;
    Expression *e = malloc(sizeof(Expression));
    *e = (Expression){EXPRESSION_VALUE,v,NULL,0,NULL};
    return e;
}
Expression* expression_init_binary(Expression *l, int op, Expression *r){
    if (l == NULL || r == NULL) return NULL;
    Expression *e = malloc(sizeof(Expression));
    *e = (Expression){EXPRESSION_BINARY,0,l,op,r};
    return e;
}
Expression* expression_init_unary(int op, Expression *r){
    if (r == NULL) return NULL;
    Expression *e = malloc(sizeof(Expression));
    *e = (Expression){EXPRESSION_UNARY,0,NULL,op,r};
    return e;
}
void expression_destroy(Expression *e) {
    puts("destroying expression");

  if (e == NULL) return;
  if (e->type == EXPRESSION_VALUE)
  {
      factor_destroy(e->value);
  }
  else if (e->type == EXPRESSION_BINARY)
  {
      expression_destroy(e->l);
      expression_destroy(e->r);
  }
  else if (e->type == EXPRESSION_UNARY)
  {
      expression_destroy(e->r);
  }
  else puts("error: unknown expression type");
  free(e);
}


Declaration* declaration_init_variable(char *name)
{
    if (name == NULL) return NULL;
    Declaration *d = malloc(sizeof(Declaration));
    *d = (Declaration){DECLARATION_VARIABLE,name,NULL,NULL};
    return d;
}
Declaration* declaration_init_assignment(char *name, Expression *value)
{
    if (name == NULL) return NULL;
    Declaration *d = malloc(sizeof(Declaration));
    *d = (Declaration){DECLARATION_ASSIGNMENT,name,value,NULL};
    return d;
}
Declaration* declaration_init_function(char *name, Block *block)
{
    if (name == NULL|| block == NULL) return NULL;
    Declaration *d = malloc(sizeof(Declaration));
    *d = (Declaration){DECLARATION_FUNCTION,name,NULL,block};
    return d;
}
void declaration_destroy(Declaration* d)
{
    puts("destroying declaration");
    if (d == NULL) return;
    if (d->kind == DECLARATION_VARIABLE || d->kind == DECLARATION_ASSIGNMENT)
    {
        free(d->name);
        free(d);
    }
    else if (d->kind == DECLARATION_FUNCTION)
    {
        free(d->name);
        block_destroy(d->block);
        free(d);
    }
    else
    {
        puts("error: unknown declaration type");
        free(d);
    }
    puts("finish destroying declaration");
}

Statement* statement_init_with_expression(Expression *e) {
    if (e == NULL) return NULL;
    Statement *statement = malloc(sizeof(Statement));
    *statement = (Statement){STATEMENT_EXPRESSION, NULL, e, NULL};
    return statement;
}
Statement* statement_init_with_return(Expression *e) {
    if (e == NULL) return NULL;
    Statement *statement = malloc(sizeof(Statement));
    *statement = (Statement){STATEMENT_RETURN, NULL, e, NULL};
    return statement;
}
Statement* statement_init_with_declaration(Declaration* d)
{
    if (d == NULL) return NULL;
    Statement *statement = malloc(sizeof(Statement));
    *statement = (Statement){STATEMENT_DECLARATION, d, NULL, NULL};
    return statement;
}
Statement* statement_init_with_block(Block *b) {
  if (b==NULL) return NULL;
  Statement *statement = malloc(sizeof(Statement));
  *statement = (Statement){STATEMENT_BLOCK, NULL, NULL, b};
  return statement;
}
void statement_destroy(Statement *s) {
    puts("destroying statement");
    if (s == NULL) return;
    else if (s->type == STATEMENT_EXPRESSION) expression_destroy(s->expression);
    else if (s->type == STATEMENT_BLOCK) block_destroy(s->block);
    else if (s->type == STATEMENT_DECLARATION) declaration_destroy(s->declaration);
    else if (s->type == STATEMENT_RETURN) expression_destroy(s->expression);
    else puts("error: unknown expression type");
    free(s);
}

Block* block_init(SymbolTable *sym, Vector *v) {
    Block *b = malloc(sizeof(Block));
    *b = (Block){sym,v};
    return b;
}
void block_destroy(Block *b) {
    puts("destroying block");
    if (b == NULL) return;
    if (b->symbols == NULL) puts("error: null symbol table in block");
    if (b->statements == NULL) puts("error: null statemen vector in block");
    vector_destroy(b->statements,statement_destroy);
    symbol_table_destroy(b->symbols);
    free(b);
}

Program* program_init(SymbolTable *sym, Vector* vec) {
    Program *program = malloc(sizeof(Program));
    *program = (Program){sym,vec};
    return program;
}
void program_destroy(Program *p) {
    if (p==NULL) return;
    vector_destroy(p->declarations,declaration_destroy);
    symbol_table_destroy(p->globalSymbols);
    free(p);
}
