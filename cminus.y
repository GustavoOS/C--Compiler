%{
    #include <stdio.h>
%}

%token ELSE IF INT RETURN VOID WHILE
%token PLUS MINUS TIMES SLASH LESSER LESSEQ GREATER GREATEQ EQCOMP NOTEQ EQATR SEMI COMMA LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE
%token ID NUM
%nonassoc "then"
%nonassoc ELSE

%%

programa: 
        declaracao-lista
        ;

declaracao-lista: 
        declaracao-lista declaracao 
        | declaracao
        ;

declaracao: 
        var-declaracao 
        | fun-declaracao
        ;

var-declaracao: 
        tipo-especificador ID SEMI
        |tipo-especificador ID LBRACE NUM RBRACE SEMI
        ;


tipo-especificador: 
        INT 
        | VOID 
        ;

fun-declaracao: 
        tipo-especificador ID LPAREN params RPAREN composto-decl
        ;

params:
        param-lista 
        | VOID
        ;

param-lista: 
        param-lista COMMA param 
        | param
        ;

param: 
        tipo-especificador ID 
        | tipo-especificador ID LBRACE RBRACE
        ;

composto-decl: 
        LBRACKET local-declaracoes statement-lista RBRACKET
        ;

local-declaracoes: 
        local-declaracoes var-declaracao 
        | %empty
        ;

statement-lista: 
        statement-lista statement 
        | %empty
        ;

statement: 
        expressao-decl 
        | composto-decl 
        | selecao-decl 
        | iteracao-decl 
        | retorno-decl
        ;

expressao-decl: 
        expressao SEMI 
        | SEMI
        ;

selecao-decl: 
        IF LPAREN expressao RPAREN statement %prec "then"
        | IF LPAREN expressao RPAREN statement ELSE statement
        ;

iteracao-decl: 
        WHILE LPAREN expressao RPAREN statement
        ;

retorno-decl: 
        RETURN SEMI 
        | RETURN expressao SEMI
        ;

expressao: 
        var EQATR expressao 
        | simples-expressao
        ;

fator: 
        LPAREN expressao RPAREN 
        | var 
        | ativacao 
        | NUM
        ;


var: 
        ID
        | ID LBRACE expressao RBRACE
        ;


simples-expressao: 
        soma-expressao relacional soma-expressao 
        | soma-expressao
        ;

relacional: 
        LESSEQ 
        | LESSER 
        | GREATER 
        | GREATEQ
        | EQCOMP 
        | NOTEQ
        ;

soma-expressao: 
        soma-expressao soma termo 
        | termo
        ;

soma: 
        PLUS 
        | MINUS
        ;

termo: 
        termo mult fator 
        | fator
        ;

mult: 
        TIMES 
        | SLASH
        ;


ativacao: 
        ID LPAREN args RPAREN
        ;

args: 
        arg-lista 
        | %empty
        ;

arg-lista: 
        arg-lista COMMA expressao 
        | expressao
        ;

%%
