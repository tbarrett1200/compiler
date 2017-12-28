#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <parser.h>
#include <debug.h>
#include <color.h>

int main(int argc, char const *argv[])
{
    FILE *in = fopen("new.txt","r");
    FILE *out = fopen("new.ll","w");

    if (in == NULL || out == NULL)
    {
        puts(RED "error: file not specified" RESET);
        return 1;
    }

    Program *p = parse_program(in);
    emit_program(out,p);
    program_destroy(p);
    fclose(in);
    fclose(out);
    system("clang new.ll -o new");
    system("./new || echo $?");

    return 0;
}
