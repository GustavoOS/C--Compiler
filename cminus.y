%{
    #include <stdio.h>SEMI
%}

%token INT

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
        tipo-especificador ID SEMI 
        | tipo-especificador ID [ NUM ] SEMI
        ;
tipo-especificador : 
        int 
        | void 
        | float
        ;
fun-declaracao : 
        tipo-especificador ID ( params ) composto-decl
        ;
params :
        param-lista 
        | void
        ;
param-lista : 
        param-lista, param 
        | param
        ;
param : 
        tipo-especificador ID 
        | tipo-especificador ID [ ]
        ;
composto-decl : 
        { local-declaracões statement-lista }
        ;
local-declaracões : 
        local-declaracões var-declaracao 
        | vazio
        ;
statement-lista : 
        statement-lista statement 
        | vazio
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
        if ( expressao ) statement 
        | if ( expressao ) statement else statement
        ;
iteracao-decl : 
        while ( expressao ) statement
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
        | ID [ expressao ]
simples-expressao : 
        soma-expressao relacional soma-expressao 
        | soma-expressao
        ;
relacional : 
        <= 
        | < 
        | > 
        | >= 
        | == 
        | !=
        ;
soma-expressao : 
        soma-expressao soma termo 
        | termo
soma : 
        + 
        | -
        ;
termo : 
        termo mult fator 
        | fator
        ;
mult : 
        * 
        | /
        ;
fator : 
        ( expressao ) 
        | var 
        | ativacao 
        | NUM
        ;
ativacao : 
        ID ( args )
        ;
args : 
        arg-lista 
        | vazio
        ;
arg-lista : 
        arg-lista COMMA expressao 
        | expressao
        ;

%%
