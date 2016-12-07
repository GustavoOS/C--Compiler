%{
    #define YYPARSER /* distinguishes Yacc output from other code files */

    #include "globals.h"
    #include "util.h"
    #include "scan.h"
    #include "parse.h"

    #define YYSTYPE TreeNode *
    static char * savedName; /* for use in assignments */
    static int savedConstant;
    static int savedType;
    static int savedLineNo;  /* ditto */
    static TreeNode * savedTree; /* stores syntax tree for later return */
    static int yylex(void);
    int yyerror(char * message);

%}

%token ERROR ELSE IF INT RETURN VOID WHILE
%token PLUS MINUS TIMES SLASH LESSER LESSEQ GREATER GREATEQ EQCOMP NOTEQ EQATR SEMI COMMA LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE
%token ID NUM
%nonassoc "then"
%nonassoc ELSE

%%

programa:
        declaracao-lista 
                {savedTree=$1;}
        | %empty
        {       savedTree=NULL;
        }
        ;

declaracao-lista:
        declaracao-lista declaracao
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $2;
                     $$ = $1; }
                     else $$ = $2;
                 }
        | %empty { $$ = NULL; }
        ;

declaracao:
        var-declaracao {$$ = $1;}
        | fun-declaracao  {$$ = $1;}
        ;

var-declaracao:
        tipo-especificador ID
                { savedName = copyString(tokenString);
                  savedLineNo = lineno;}
        SEMI 
                { $$ = newStmtNode(VarDeclK);
                  $$->attr.name = savedName;
                  $$->type = savedType;
                }
        |tipo-especificador ID
                { savedName = copyString(tokenString);
                  savedLineNo = lineno;          
                }
        LBRACE NUM
                { savedConstant = atoi(tokenString); }
        RBRACE SEMI
                { $$ = newStmtNode(VarDeclK);
                  $$->attr.name = savedName;
                  $$->child[0] = newExpNode(ConstK);
                  $$->child[0]->attr.val = savedConstant; 
                  $$->type = savedType;
                }
        ;


tipo-especificador:
        INT
        {
                savedType=Integer;
        }
        | VOID
        {
                savedType=Void;
        }
        ;

fun-declaracao:
        tipo-especificador ID
        {
                // printf("Chegou no ID %s\n" ,tokenString);
                $$ = newStmtNode(FunDeclK);
                $$->attr.name =  copyString(tokenString);;
        } 
        LPAREN params RPAREN composto-decl
                { 
                  $$ = $3;
                  $$->type = savedType;
                  $$->child[0] = $5;
                  $$->child[1] = $7;
                }
        ;

params:
        param-lista      { $$ = $1;  }
        | VOID
        ;

param-lista:
        param-lista COMMA param
        { YYSTYPE t = $1;
                if (t != NULL)
                { while (t->sibling != NULL)
                        t = t->sibling;
                t->sibling = $3;
                $$ = $1; }
                else $$ = $3;
        }
        | param  { $$=$1; }
        ;

param:
        tipo-especificador ID
                { 
                  $$ = newStmtNode(VarDeclK);
                  $$->attr.name = copyString(tokenString);
                  $$->type = savedType;
                //   printf("Chegou no param %s\n",tokenString);
                }
        | tipo-especificador ID 
                { savedName = copyString(tokenString);
                  savedLineNo = lineno;
                //   printf("Chegou no param %s\n",tokenString);
                }
        LBRACE RBRACE
                { $$ = newStmtNode(VarDeclK);
                  $$->attr.name = savedName;
                  $$->type = savedType;
                }
        | error  {
                $$=NULL;
                printf("DEU RUIM\n");}
        ;

composto-decl:
        LBRACKET local-declaracoes statement-lista RBRACKET
        { 
                  $$ = newStmtNode(CompoundK);
                  $$->child[0] = $2;
                  $$->child[1] = $3;
                //   printf("Chegou no composto \n");
                //   printTree($3);
        }
        ;

local-declaracoes:
        local-declaracoes var-declaracao
        {       YYSTYPE t = $1;
                if (t != NULL)
                { while (t->sibling != NULL)
                        t = t->sibling;
                t->sibling = $2;
                $$ = $1; 
                }
                else $$ = $2;
        }
        | %empty{$$=NULL;}
        ;

statement-lista:
        statement-lista statement
        {       YYSTYPE t = $1;
                if (t != NULL)
                { while (t->sibling != NULL)
                        t = t->sibling;
                t->sibling = $2;
                $$ = $1; 
                }
                else $$ = $2;
        }
        | %empty {$$=NULL;}
        ;

statement:
        expressao-decl
        | composto-decl
        | selecao-decl
        | iteracao-decl
        | retorno-decl
        | error {$$=NULL;}
        ;

expressao-decl:
        expressao SEMI
        | SEMI {$$=NULL;}
        ;

selecao-decl:
        IF LPAREN expressao RPAREN statement %prec "then"
        {       $$ = newStmtNode(IfK);
                $$->child[0] = $3;
                $$->child[1] = $5;
                // printf("Chegou no IF \n");
        }
        | IF LPAREN expressao RPAREN statement ELSE statement
        {       $$ = newStmtNode(IfK);
                $$->child[0] = $3;
                $$->child[1] = $5;
                $$->child[2] = $7;
                // printf("Chegou no IF \n");
        }
        ;

iteracao-decl:
        WHILE LPAREN expressao RPAREN statement
        {       $$ = newStmtNode(WhileK);
                $$->child[0] = $3;
                $$->child[1] = $5;
                // printf("Chegou no WHILE \n");
        }
        ;

retorno-decl:
        RETURN SEMI
        {       $$ = newStmtNode(ReturnK);
                // printf("Chegou no RETURN \n");
        }
        | RETURN expressao SEMI
        {       $$ = newStmtNode(ReturnK);
                $$->child[0] = $2;
                // printf("Chegou no RETURN \n");
        }
        ;

expressao:
        var EQATR expressao
        {       $$ = newStmtNode(AssignK);
                $$->child[0] = $1;
                $$->child[1] = $3;
                // printf("Chegou no expressao \n");
        }
        | simples-expressao
        ;

fator:
        LPAREN expressao RPAREN
        {
                $$=$2;
        }
        | var
        | ativacao
        | NUM {
                $$=newExpNode(ConstK);
                $$->attr.val=atoi(tokenString);
        }
        | error {$$=NULL;}
        ;


var:
        ID{ 
                  $$ = newExpNode(IdK);
                  $$->attr.name = copyString(tokenString);
                //   printf("Chegou no var ");
                }
        | ID LBRACE expressao RBRACE
        { 
                  $$ = newExpNode(IdK);
                  $$->attr.name = copyString(tokenString);
                  $$->child[0]=$3;
                //   printf("Chegou no var ");
        }
        ;


simples-expressao:
        soma-expressao relacional soma-expressao
        { 
                  $$ = $2;
                  $$->child[0]=$1;
                  $$->child[1]=$3;
                //   printf("Chegou no simples-expressao ");
        }
        | soma-expressao
        ;

relacional:
        LESSEQ
        {
                  $$ = newExpNode(OpK);
                  $$->attr.op = LESSEQ;
        }
        | LESSER
        {
                  $$ = newExpNode(OpK);
                  $$->attr.op = LESSER;
        }
        | GREATER
        {
                  $$ = newExpNode(OpK);
                  $$->attr.op = GREATER;
        }
        | GREATEQ
        {
                  $$ = newExpNode(OpK);
                  $$->attr.op = GREATEQ;
        }
        | EQCOMP
        {
                  $$ = newExpNode(OpK);
                  $$->attr.op = EQCOMP;
        }
        | NOTEQ
        {
                  $$ = newExpNode(OpK);
                  $$->attr.op = NOTEQ;
        }
        ;

soma-expressao:
        soma-expressao soma termo
        { 
                  $$ = $2;
                  $$->child[0]=$1;
                  $$->child[1]=$3;
                //   printf("Chegou no soma-expressao ");
        }
        | termo
        ;

soma:
        PLUS
        {
                  $$ = newExpNode(OpK);
                  $$->attr.op = PLUS;
        }
        | MINUS
        {
                  $$ = newExpNode(OpK);
                  $$->attr.op = MINUS;
        }
        ;

termo:
        termo mult fator
        { 
                  $$ = $2;
                  $$->child[0]=$1;
                  $$->child[1]=$3;
                //   printf("Chegou no termo ");
        }
        | fator
        ;

mult:
        TIMES
        {
                  $$ = newExpNode(OpK);
                  $$->attr.op = TIMES;
        }
        | SLASH
        {
                  $$ = newExpNode(OpK);
                  $$->attr.op = SLASH;
        }
        ;


ativacao:
        ID 
        {
                $$ = newStmtNode(FunActiveK);
                $$->attr.name= copyString(tokenString);
        }
        LPAREN args RPAREN
        {       $$ = $2;
                $$->child[0] = $4;
                // printf("Chegou no ativacao \n");
        }
        ;

args:
        arg-lista
        | %empty {$$=NULL;}
        ;

arg-lista:
        arg-lista COMMA expressao
        {       YYSTYPE t = $1;
                if (t != NULL)
                { while (t->sibling != NULL)
                        t = t->sibling;
                t->sibling = $3;
                $$ = $1; 
                }
                else $$ = $3;
        }
        | expressao
        ;

%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}
