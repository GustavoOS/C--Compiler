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

tipo-especificador : 
        INT 
        | VOID 
        ;

fun-declaracao : 
        tipo-especificador ID LPAREN params RPAREN composto-decl
        ;

params :
        param-lista 
        | VOID
        ;

param-lista : 
        param-lista, param 
        | param
        ;

param : 
        tipo-especificador ID 
        | tipo-especificador ID LBRACE RBRACE
        ;

composto-decl : 
        LBRACKET local-declaracões statement-lista RBRACKET
        ;

local-declaracões : 
        local-declaracões var-declaracao 
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
        if LPAREN expressao RPAREN statement 
        | if LPAREN expressao RPAREN statement else statement
        ;

iteracao-decl : 
        while LPAREN expressao RPAREN statement
        ;

retorno-decl : 
        return SEMI 
        | return expressao SEMI
        ;

expressao : 
        var = expressao 
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
