#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ast.h>
#include <symbol.h>
#include <container/hash.h>

int hash(char *c)
{
    int length = strlen(c);
    int ret = 345;
    for (int i=0; i<length; i++) ret += c[i];
    return ret%256;
}


Identifier* identifier_init(int kind)
{
    Identifier *i = malloc(sizeof(Identifier));
    *i = (Identifier){kind,0,0};
    return i;
}
void identifier_destroy(Identifier* i)
{
    free(i);
}

SymbolTable* symbol_table_init(SymbolTable *parent)
{
    SymbolTable *s = malloc(sizeof(SymbolTable));
    *s = (SymbolTable){parent, hash_init(hash,strcmp),0};
    return s;
}

void symbol_table_set(SymbolTable *t,char *key, Identifier *value)
{
    if (t==NULL || key == NULL || value == NULL) puts("error: sym table null");
    hash_set(t->table, key, value);
}

Identifier* symbol_table_get_current_only(SymbolTable *t,char *key)
{
    return hash_get(t->table,key);
}


Identifier* symbol_table_get(SymbolTable *t,char *key)
{
    Identifier* i = NULL;

    while (i == NULL && t != NULL)
    {
        i = hash_get(t->table,key);
        t = t->parent;
    }

    return i;
}
void symbol_table_destroy(SymbolTable *h)
{
    if (h == NULL) return;
    hash_destroy(h->table,NULL,free);
    free(h);
}
