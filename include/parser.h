#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdbool.h>
#include <ast.h>

typedef struct Operator
{
    int token;
    char *string;
    int precedence;
    int type;
} Operator;

#define OPERATOR_INFIX_LEFT 1
#define OPERATOR_INFIX_RIGHT 2
#define OPERATOR_INFIX_NONE 4
#define OPERATOR_PREFIX 8
#define OPERATOR_POSTFIX 16


#define MAX_PRECEDENCE 4
#define NUM_OPERATIONS 8
extern Operator operations[NUM_OPERATIONS];

bool hasOperatorAtPrecedence(int prec);
Operator* operatorInfo(int op, int type);

Expression* parse_prefix_expression(SymbolTable*,int precedence);
Expression* parse_postfix_expression(SymbolTable*,int precedence);
Expression* parse_postfix_expression2(SymbolTable*,int precedence, Expression *base);
Expression* parse_left_associative_infix_expression2(SymbolTable*,int precedence, Expression* left);

Factor* parse_factor(SymbolTable*);
Expression* parse_expression(SymbolTable*,int precedence);
Statement* parse_statement(SymbolTable*);
Block* parse_block(SymbolTable*);
Program* parse_program(FILE *f);

#endif
