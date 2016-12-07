%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
static int yylex(void);
%}

%x comment
%x comentario


%%
"/*"         BEGIN(comment);
<comment>[^*\n]*                    /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*               /* eat up '*'s not followed by '/'s */
<comment>\n                         ++lineno;
<comment>"*"+"/"                    BEGIN(INITIAL);

"//"         BEGIN(comentario);
<comentario>[^\n]*
<comentario>\n                         ++lineno;BEGIN(INITIAL);



"else"                              return ELSE;
"if"                                return IF;
"int"                               return INT;
"return"                            return RETURN;
"void"                              return VOID;
"while"                             return WHILE;

"_"?[a-zA-Z][_a-zA-Z0-9]*           return ID;
[0-9]+                              return NUM;

[ \t]                               ;
\n                                  lineno++;

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
"{"                                 return LBRACKET;
"}"                                 return RBRACKET;
"["                                 return LBRACE;
"]"                                 return RBRACE;


.                                   return ERROR;

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}


