
# Gramática BNF para C-:

programa → declaração-lista
declaração-lista → declaração-lista declaração | declaracao
declaração → var-declaração | fun-declaração
declaração → error <===== MODIFICAÇÃO
var-declaração → tipo-especificador ID ; | tipo-especificador ID [ NUM ] ;
tipo-especificador → int | void 
fun-declaração → tipo-especificador ID ( params ) composto-decl
params → param-lista | void
params → error <======= MODIFICAÇÃO
param-lista → param-lista, param | param
param → tipo-especificador ID | tipo-especificador ID [ ]
composto-decl → { local-declarações statement-lista }
local-declarações → local-declarações var-declaração | vazio
statement-lista → statement-lista statement | vazio
statement → expressão-decl | composto-decl | seleção-decl | iteração-decl | retorno-decl
expressão-decl → expressão ; | ;
seleção-decl → if ( expressão ) statement | if ( expressão ) statement else statement
iteração-decl → while ( expressão ) statement
retorno-decl → return ; | return expressão;
expressão → var = expressão | simples-expressão
var → ID | ID [ expressão ]
simples-expressão → soma-expressão relacional soma-expressão | soma-expressão
relacional → <= | < | > | >= | == | !=
soma-expressão → soma-expressão soma termo | termo
soma → + | -
termo → termo mult fator | fator
mult → * | /
fator → ( expressão ) | var | ativação | NUM
ativação → ID ( args )
args → arg-lista | vazio
arg-lista → arg-lista , expressão | expressão


# EXEMPLO (SUGESTÃO) DE ÁRVORE SINTÁTICA:
Programa de entrada:
1 
 int gcd(int u, int v)
2 
{ if (v==0) return u;
3 
 else return gcd(v, u-u/v*v);
4 
}
5  
void main(void)
6 
{ int x; int y;
7 
  x = input(); y = input();
8 
  output(gcd(x,y));
9 
}
