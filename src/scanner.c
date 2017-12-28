#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <scanner.h>

int number;
char *name;

FILE *in = NULL;
int c;
Token *out;

Token scan_number(void);
Token scan_name(void);
Token scan_unknown(void);

Token nextToken(void)
{
    if (in == NULL) in = stdin;

    while(1)
    {
        do { c = fgetc(in); } while(isspace(c));

        if (c == '+') return ADD;
        else if (c == '-') return SUBTRACT;
        else if (c == '*') return MULTIPLY;
        else if (c == '/') return DIVIDE;
        else if (c == '%') return MODULUS;
        else if (c == '=') return EQUALS;


        else if (c == '(') return LPAREN;
        else if (c == ')') return RPAREN;
        else if (c == '{') return LBRACE;
        else if (c == '}') return RBRACE;
        else if (c == ';') return SEMICOLON;
        else if (c == ':') return COLON;

        else if (isdigit(c)) return scan_number();
        else if (isalpha(c)) return scan_name();
        else if (c == EOF) return END;
        else return scan_unknown();
    }
}

Token scan_number(void)
{
    char string[16] = {0};

    for(int i=0; i<15; i++) {
        string[i]=c;
        c = fgetc(in);
        if (!isdigit(c)) break;
    }

    while (isdigit(c)) {
        printf("error: number overflow");
        c = fgetc(in);
    }

    ungetc(c,in);

    number = atoi(string);

    return NUMBER;
}

Token scan_name(void) {

    name = calloc(32,1);

    for (int i=0; i<31; i++) {
        name[i]=c;
        c=fgetc(in);
        if (!isalnum(c)) break;
    }

    while (isalnum(c)) {
        printf("error: identifier too long");
        c = fgetc(in);
    }

    ungetc(c,in);

    if (strcmp(name,"var") == 0) return VAR;
    if (strcmp(name,"function") == 0) return FUNCTION;
    if (strcmp(name,"return") == 0) return RETURN;
    else return NAME;
}

Token scan_unknown(void)
{
    name = calloc(32,1);

    for(int i=0; i<31; i++) {
        name[i]=c;
        c=fgetc(in);
        if (isspace(c)) {
            ungetc(c,in);
            break;
        }
    }

    printf("error: unknown token '%s'\n",name);
    free(name);
    return UNKNOWN;
}

void scanner_init(FILE *f, Token *t)
{
    in = f;
    out = t;
    next();
}

void next(void)
{
    *out = nextToken();
}
