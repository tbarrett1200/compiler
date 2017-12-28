#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <container/hash.h>

typedef struct Identifier
{
    int kind;
    int global;
    int varnum;
} Identifier;

#define IDENTIFIER_VARIABLE 0
#define IDENTIFIER_FUNCTION 1
#define IDENTIFIER_TYPE 2

typedef struct SymbolTable
{
    struct SymbolTable *parent;
    Hash *table;
    int varnum;
} SymbolTable;


Identifier* identifier_init(int kind);
void identifier_destroy(Identifier*);

SymbolTable* symbol_table_init();
void symbol_table_destroy(SymbolTable *);

void symbol_table_set(SymbolTable *t,char *key,struct Identifier *value);
Identifier* symbol_table_get(SymbolTable *t,char *key);
Identifier* symbol_table_get_current_only(SymbolTable *t,char *key);

#endif
