%{
#include "scanner.h"
%}

%x comment


%%
        int line_num = 1;

"/*"         BEGIN(comment);

<comment>[^*\n]*                    /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*               /* eat up '*'s not followed by '/'s */
<comment>\n                         ++line_num;
<comment>"*"+"/"                    BEGIN(INITIAL);

"else"                              return ELSE;
"if"                                return IF;
"int"                               return INT;
"return"                            return RETURN;
"void"                              return VOID;
"while"                             return WHILE;

"_"?[a-zA-Z][_a-zA-Z0-9]*           return ID;
[0-9]+                              return NUM;

[ \t\n]                             ;


"+"                                 return PLUS;
"-"                                 return MINUS;
"*"                                 return TIMES;
"/"                                 return SLASH;
"<"                                 return LESSER;
"<="                                return LESSEQ;
">"                                 return GREATER;
">="                                return GREATEQ;
"=="                                return EQCOMP;
"!="                                return NOTEQ;
"="                                 return EQATR;
";"                                 return SEMI;
","                                 return COMMA;
"("                                 return LPAREN;
")"                                 return RPAREN;
"["                                 return LBRACKET;
"]"                                 return RBRACKET;
"{"                                 return LBRACE;
"}"                                 return RBRACE;


.                                   return ERRO;

%%


int yywrap(void){
    return 1;
}
