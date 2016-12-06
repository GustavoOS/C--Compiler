%{
    #include <stdio.h>
%}

%token ELSE IF INT RETURN VOID WHILE
%token PLUS MINUS TIMES SLASH LESSER LESSEQ GREATER GREATEQ EQCOMP NOTEQ EQATR SEMI COMMA LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE
%token ID NUM

%%

programa : 
        declaracao-lista
        ;

declaracao-lista : 
        declaracao-lista declaracao 
        | declaracao
        ;

declaracao : 
        var-declaracao 
        | fun-declaracao
        ;

var-declaracao : 
        INT ID SEMI 
        | INT ID LBRACE NUM RBRACE SEMI
        ;

tipo-especIFicador : 
        INT 
        | VOID 
        ;

fun-declaracao : 
        tipo-especIFicador ID LPAREN params RPAREN composto-decl
        ;

params :
        param-lista 
        | VOID
        ;

param-lista : 
        param-lista COMMA param 
        | param
        ;

param : 
        tipo-especIFicador ID 
        | tipo-especIFicador ID LBRACE RBRACE
        ;

composto-decl : 
        LBRACKET local-declaracoes statement-lista RBRACKET
        ;

local-declaracoes : 
        local-declaracoes var-declaracao 
        | %empty
        ;

statement-lista : 
        statement-lista statement 
        | %empty
        ;

statement : 
        expressao-decl 
        | composto-decl 
        | selecao-decl 
        | iteracao-decl 
        | retorno-decl
        ;

expressao-decl : 
        expressao SEMI 
        | SEMI
        ;

selecao-decl : 
        IF LPAREN expressao RPAREN statement 
        | IF LPAREN expressao RPAREN statement ELSE statement
        ;

iteracao-decl : 
        WHILE LPAREN expressao RPAREN statement
        ;

retorno-decl : 
        RETURN SEMI 
        | RETURN expressao SEMI
        ;

expressao : 
        var EQATR expressao 
        | simples-expressao
        ;

var : 
        ID 
        | ID LBRACE expressao RBRACE
        ;

simples-expressao : 
        soma-expressao relacional soma-expressao 
        | soma-expressao
        ;

relacional : 
        LESSEQ 
        | LESSER 
        | GREATER 
        | GREATEQ
        | EQATR 
        | NOTEQ
        ;

soma-expressao : 
        soma-expressao soma termo 
        | termo
        ;

soma : 
        PLUS 
        | MINUS
        ;

termo : 
        termo mult fator 
        | fator
        ;

mult : 
        TIMES 
        | SLASH
        ;

fator : 
        LPAREN expressao RPAREN 
        | var 
        | ativacao 
        | NUM
        ;

ativacao : 
        ID LPAREN args RPAREN
        ;

args : 
        arg-lista 
        | %empty
        ;

arg-lista : 
        arg-lista COMMA expressao 
        | expressao
        ;

%%
