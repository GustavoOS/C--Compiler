%{
#include "globals.h"
#include "util.h"
#include "scan.h"
#include <iostream>
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
static int yylex(void);
%}

%Start MultiLineComment SingleLineComment


%%
<INITIAL>"/*"                         BEGIN MultiLineComment;
<MultiLineComment>"*"+\/              BEGIN (INITIAL);
<MultiLineComment>[^\n\*]*            |
<MultiLineComment>"*"+[^\n\/]         ;//Deletes most characters
<MultiLineComment>\n                  lineno++;
<MultiLineComment>.                   ;//Deletes missing characters if needed
<MultiLineComment><<EOF>>             ;// Ignore


<INITIAL>"//"                         BEGIN SingleLineComment;
<SingleLineComment>.*                 ;
<SingleLineComment>\n                 lineno++;BEGIN (INITIAL);
<SingleLineComment><<EOF>>            BEGIN (INITIAL);



"else"                              return ELSE;
"ELSE"                              return ELSE;
"if"                                return IF;
"IF"                                return IF;
"int"                               return INT;
"INT"                               return INT;
"return"                            return RETURN;
"RETURN"                            return RETURN;
"void"                              return VOID;
"VOID"                              return VOID;
"while"                             return WHILE;
"WHILE"                             return WHILE;

[_@:]?[a-zA-Z][_a-zA-Z0-9]*         return ID;
[0-9]+                              return NUM;

[ \t]                               ;
\n                                  lineno++;
\r                                  ;
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


.                                   std::cout << "\nERROR: Uncaught character at line " << lineno <<"\n"; return ERROR;

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
  if( currentToken == ID || currentToken== NUM)
    strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,yytext);
  }
  return currentToken;
}


