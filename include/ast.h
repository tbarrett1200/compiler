#ifndef AST_H
#define AST_H

#include <container/vector.h>
#include <container/hash.h>
#include <symbol.h>

struct Factor;
struct Expression;
struct Declaration;
struct Statement;
struct Block;
struct Program;

typedef struct Factor
{
    int type;
    int integer;
    char *identifier;
    struct Expression *expression;
} Factor;

#define FACTOR_INTEGER 1
#define FACTOR_IDENTIFIER 2
#define FACTOR_EXPRESSION 3

typedef struct Expression
{
    int type;
    struct Factor *value;
    struct Expression *l;
    int op;
    struct Expression *r;
} Expression;

#define EXPRESSION_VALUE 1
#define EXPRESSION_UNARY 2
#define EXPRESSION_BINARY 3

typedef struct Declaration
{
    int kind;
    char *name;
    Expression *value;
    struct Block *block;
} Declaration;

#define DECLARATION_VARIABLE 1
#define DECLARATION_ASSIGNMENT 2
#define DECLARATION_FUNCTION 3

typedef struct Statement
{
    int type;
    struct Declaration *declaration;
    struct Expression *expression;
    struct Block *block;
} Statement;

#define STATEMENT_EXPRESSION 1
#define STATEMENT_BLOCK 2
#define STATEMENT_DECLARATION 3
#define STATEMENT_RETURN 4


typedef struct Block
{
    SymbolTable *symbols;
    Vector *statements;
} Block;

typedef struct Program
{
    SymbolTable *globalSymbols;
    Vector *declarations;
} Program;

Factor* factor_init_with_integer(int);
Factor* factor_init_with_identifier(char*);
Factor* factor_init_with_expression(Expression*);
void factor_destroy(Factor*);

Expression* expression_init_with_value(Factor *value);
Expression* expression_init_binary(Expression *l, int op, Expression *r);
Expression* expression_init_unary(int op, Expression *e);
void expression_destroy(Expression*);

Declaration* declaration_init_variable(char *name);
Declaration* declaration_init_assignment(char *name, Expression*);
Declaration* declaration_init_function(char *name, Block *block);
void declaration_destroy(Declaration*);

Statement* statement_init_with_expression(Expression*);
Statement* statement_init_with_declaration(Declaration*);
Statement* statement_init_with_block(Block*);
Statement* statement_init_with_return(Expression*);
void statement_destroy(Statement*);

Block* block_init(SymbolTable *,Vector*);
void block_destroy(Block*);

Program* program_init(SymbolTable*, Vector*);
void program_destroy(Program*);

#endif
