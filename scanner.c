#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"

extern int yylex();
extern int yylineno;
extern char* yytext;

char *names[]={NULL,"else", "if", "int", "return", "void", "while", "+", "-", "*", "/", "<", "<=", ">", ">=", "==", "!=", "=", ";", ",", "(", ")", "[", "]", "{", "}", "id", "num" };

int main(void){
    
    int ntoken, vtoken;

    ntoken=yylex();
    while(ntoken!=0){
        printf("%d\n", ntoken);
        ntoken=yylex();
    }
    return 0;

}