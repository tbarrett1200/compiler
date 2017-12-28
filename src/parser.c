#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <container/vector.h>
#include <container/hash.h>
#include <parser.h>
#include <ast.h>
#include <scanner.h>
#include <symbol.h>

Token token;

Operator operations[NUM_OPERATIONS] =
{
    (Operator){EQUALS,"=",4,OPERATOR_INFIX_NONE},
    (Operator){ADD,"+",3,OPERATOR_INFIX_LEFT},
    (Operator){SUBTRACT,"-",3,OPERATOR_INFIX_LEFT},
    (Operator){MULTIPLY,"*",2,OPERATOR_INFIX_LEFT},
    (Operator){DIVIDE,"/",2,OPERATOR_INFIX_LEFT},
    (Operator){MODULUS,"%",2,OPERATOR_INFIX_LEFT},
    (Operator){ADD,"+",1,OPERATOR_PREFIX},
    (Operator){ADD,"-",1,OPERATOR_PREFIX},
};

bool check(Token t) {
    return (token == t);
}
bool accept(Token t) {
    if (token == t)
    {
        next();
        return true;
    }
    else return false;
}
Factor* parse_factor(SymbolTable *parent) {

    Factor *factor = NULL;

    if (check(NUMBER))
    {
        factor = factor_init_with_integer(number);
        next();
    }
    else if (check(NAME))
    {
        char *idt = name;
        next();
        if(symbol_table_get(parent,idt)==NULL)
        {
            printf("error: undeclared identifier %s\n", idt);
            return NULL;
        }
        else return factor_init_with_identifier(idt);
    }
    else if (accept(LPAREN))
    {
        Expression *e = parse_expression(parent, MAX_PRECEDENCE);
        if (e == NULL)  puts("error: expected expression");
        if (!accept(RPAREN)) puts("error: expected ')'");
        factor = factor_init_with_expression(e);
    }

    return factor;
}
bool hasOperatorAtPrecedence(int p) {

    bool accept = false;

    for (int i=0; i<NUM_OPERATIONS; i++)
    {
        if (operations[i].precedence == p)
        {
            if (operations[i].token == token)
            {
                accept = true;
                break;
            }
        }
    }

    return accept;
}
int associativityAtPrecedence(int p){
    for (int i=0; i<NUM_OPERATIONS; i++)
    {
        if (operations[i].precedence == p)
        {
            return operations[i].type;
        }
    }
    return -1;
}
Operator* operatorInfo(int token, int type){
    for (int i=0; i<NUM_OPERATIONS; i++)
    {
        if (operations[i].token == token && (operations[i].type & type) != 0)
        {
            return &(operations[i]);
        }
    }
    return NULL;
}
Expression* parse_prefix_expression(SymbolTable *parent, int precedence)
{
    if (hasOperatorAtPrecedence(precedence)) {
         int op = token;
         next();
         Expression *e = parse_expression(parent,precedence);
         return expression_init_unary(op,e);
    } else return parse_expression(parent,precedence-1);
}
Expression* parse_postfix_expression(SymbolTable *parent, int precedence)
{
    Expression *base = parse_expression(parent,precedence-1);

    if (hasOperatorAtPrecedence(precedence)) {
         int op = token;
         next();
         Expression *current = expression_init_unary(op, base);
         return parse_postfix_expression2(parent,precedence,current);
    } else return base;
}
Expression* parse_postfix_expression2(SymbolTable *parent, int precedence, Expression *base)
{
    int op;

    if (hasOperatorAtPrecedence(precedence)) {
        op = token;
        next();
    } else return base;

    Expression *current = expression_init_unary(op, base);
    return parse_postfix_expression2(parent, precedence,current);
}
Expression* parse_right_associative_infix_expression(SymbolTable *parent, int precedence) {
    if (precedence == 0) return expression_init_with_value(parse_factor(parent));

    Expression* l = parse_expression(parent,precedence-1);

    int op;

    if (hasOperatorAtPrecedence(precedence)) { op = token; next(); }
    else return l;

    Expression* r = parse_expression(parent,precedence);

    return expression_init_binary(l,op,r);
}
Expression* parse_left_associative_infix_expression(SymbolTable *p, int precedence) {
  if (precedence == 0) return expression_init_with_value(parse_factor(p));
  else
  {
      //parse tree of all expressions of lower precedence
      Expression* end = parse_expression(p,precedence-1);

      //the parent of the expression in the parse tree
      Expression* parent = parse_left_associative_infix_expression2(p,precedence,end);

      //if there are no operations at this precedence return a lower precedence
      if (parent == NULL) return end;
      else return parent;
  }
}
Expression* parse_left_associative_infix_expression2(SymbolTable *p, int precedence, Expression* left){

    int op;

    if (hasOperatorAtPrecedence(precedence)) { op = token; next(); }
    else return NULL;

    Expression *right = parse_expression(p, precedence-1);
    Expression *self = expression_init_binary(left,op,right);
    Expression *parent = parse_left_associative_infix_expression2(p, precedence,self);

    if (parent == NULL) return self;
    else return parent;
}
Expression* parse_non_associative_infix_expression(SymbolTable *parent, int precedence) {
    if (precedence == 0) return expression_init_with_value(parse_factor(parent));

    Expression *l = parse_expression(parent,precedence-1);
    int op;

    if (hasOperatorAtPrecedence(precedence)) { op = token; next(); }
    else return l;

    Expression *r = parse_expression(parent, precedence-1);

    return expression_init_binary(l,op,r);
}
Expression* parse_expression(SymbolTable *parent, int p) {
    if (p==0) return expression_init_with_value(parse_factor(parent));
    else if (associativityAtPrecedence(p)==OPERATOR_INFIX_LEFT)
    {
        return parse_left_associative_infix_expression(parent,p);
    }
    else if (associativityAtPrecedence(p)==OPERATOR_INFIX_RIGHT)
    {
        return parse_right_associative_infix_expression(parent,p);
    }
    else if (associativityAtPrecedence(p)==OPERATOR_INFIX_NONE)
    {
        return parse_non_associative_infix_expression(parent,p);
    }
    else if (associativityAtPrecedence(p)==OPERATOR_PREFIX)
    {
        return parse_prefix_expression(parent,p);
    }
    else if (associativityAtPrecedence(p)==OPERATOR_POSTFIX)
    {
        return parse_postfix_expression(parent,p);
    }
    else return NULL;
}
Declaration* parse_variable_declaration(SymbolTable *parent) {
    char *varType;
    char *varName;

    if (!accept(VAR)) return NULL;

    if (check(NAME)) {
        varName = name;
        next();
    } else {
        puts("error: expecting identifier");
        return NULL;
    }

    symbol_table_set(parent,varName,identifier_init(IDENTIFIER_VARIABLE));

    if (accept(EQUALS)) {
        Expression *value = parse_expression(parent,MAX_PRECEDENCE);
        return declaration_init_assignment(varName,value);
    }
    return declaration_init_variable(varName);
}
Declaration* parse_function_declaration(SymbolTable *parent) {

    char *funcType;
    char *funcName;

    if (!accept(FUNCTION)) return NULL;

    if (check(NAME)) {
        funcName = name;
        next();
    } else {
        puts("error: expecting identifier");
        return NULL;
    }

    Block *block = parse_block(parent);
    symbol_table_set(parent,funcName,identifier_init(IDENTIFIER_FUNCTION));

    return declaration_init_function(funcName, block);
}
Declaration* parse_declaration(SymbolTable *parent) {

    Declaration *d1 = parse_variable_declaration(parent);
    if (d1 != NULL) return d1;
    Declaration *d2 = parse_function_declaration(parent);
    if (d2 != NULL) return d2;

    return NULL;
}
Statement* parse_statement(SymbolTable *parent) {

    Declaration *d = parse_declaration(parent);
    if (d) return statement_init_with_declaration(d);
    Expression *e = parse_expression(parent, MAX_PRECEDENCE);
    if (e)
    {
        return statement_init_with_expression(e);
    }
    Block *b = parse_block(parent);
    if (b) return statement_init_with_block(b);
    if (accept(RETURN)) {
        Expression *e = parse_expression(parent, MAX_PRECEDENCE);
        if (e==NULL) puts("error: no return value");
        return statement_init_with_return(e);
    }
    return NULL;
}
Block* parse_block(SymbolTable *parent) {
    if (!accept(LBRACE)) return NULL;
    SymbolTable *sym = symbol_table_init(parent);
    Vector *vector = vector_init();
    Statement *s;
    while ((s = parse_statement(sym))) vector_add(vector,s);
    Block *b = block_init(sym, vector);
    if (!accept(RBRACE)) puts("error: missing closing brace");
    return b;
}

Program* parse_program(FILE *f) {
    SymbolTable *sym;
    Vector *vec;
    Declaration *dec;

    scanner_init(f,&token);
    sym = symbol_table_init(NULL);
    vec = vector_init();
    while((dec = parse_declaration(sym))) vector_add(vec,dec);
    if (!accept(END)) puts("error: expected declaration");
    return program_init(sym, vec);
}
