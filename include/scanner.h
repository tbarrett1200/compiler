#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

extern int number;
extern char *name;

typedef enum
{
    //Operators
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULUS,
    EQUALS,

    //Punctuation
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,
    COLON,

    //Keywords
    RETURN,
    FUNCTION,
    VAR,

    //Misc
    NAME,
    NUMBER,
    END,
    UNKNOWN,
} Token;

void scanner_init(FILE *f, Token *t);
void next(void);
#endif
