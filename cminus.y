%{
    #define YYPARSER /* distinguishes Yacc output from other code files */

    #include "globals.h"
    #include "util.h"
    #include "scan.h"
    #include "parse.h"

    #define YYSTYPE TreeNode *

    
    static int savedConstant;
    static ExpType savedType;
    static TreeNode * savedTree; /* stores syntax tree for later return */
    static int yylex(void);
    int yyerror( const char * message);
    int ifcount = 0;
    int whilecount = 0;

    int newWhile()
    {
        int k = whilecount;
        whilecount++;
        return k;
    }
    int newif(){
        ifcount++;
        return ifcount-1;
    }

    YYSTYPE findLastSibling(YYSTYPE node){
        while ( node != NULL && node->sibling != NULL) node = node->sibling;
        return node;
    }


%}

%token ERROR ELSE IF INT RETURN VOID WHILE
%token PLUS MINUS TIMES SLASH MOD
%token LESSER LESSEQ GREATER GREATEQ EQCOMP NOTEQ EQATR
%token LEFTSHIFT RIGHTSHIFT
%token SEMI COMMA
%token LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE
%token ID NUM
%nonassoc "then"
%nonassoc ELSE
%error-verbose 

%%

programa: declaracao-lista
            {
                savedTree=$1;
            }
        ;

declaracao-lista: declaracao-lista declaracao
                    { 
                        YYSTYPE t = $1;
                        if (t != NULL)
                        {
                            t = findLastSibling(t);
                            t->sibling = $2;
                            $$ = $1;
                        }
                        else $$ = $2;
                    }

                | %empty
                    {
                        $$ = NULL;
                    }
                ;

declaracao: var-declaracao
                {
                    $$ = $1;
                }

            | fun-declaracao
                {
                    $$ = $1;
                }
        ;

var-declaracao: tipo-especificador ID
                    {
                        $$ = newStmtNode(VarDeclK);
                        $$->attr.name = tokenString;
                        $$->type = savedType;
                    }
                    SEMI
                    {
                        $$ = $3;
                    }

                |tipo-especificador ID
                    {
                        $$ = newStmtNode(VetDeclK);
                        $$->attr.name = tokenString;
                        $$->type = savedType;
                    }
                    LBRACE NUM
                    {
                        savedConstant = atoi(tokenString);
                    }
                    RBRACE SEMI
                    {
                        $$ = $3;
                        $$->attr.val = savedConstant;

                        //Change to something like this to allow runtime allocation
                        // $$->child[0] = newExpNode(ConstK);
                        // $$->child[0]->attr.val = savedConstant; 
                    }
                ;


tipo-especificador: INT
                        {
                            savedType=Integer;
                        }
                    | VOID
                        {
                            savedType=Void;
                        }
                    ;

fun-declaracao: tipo-especificador ID
                {
                    $$ = newStmtNode(FunDeclK);
                    $$->attr.name =  "fun_" + (std::string) tokenString;
                    $$->type = savedType;
                } 
                LPAREN params RPAREN composto-decl
                { 
                    $$ = $3;
                    $$->child[0] = $5;
                    $$->child[1] = $7;
                }
                ;

params: param-lista
            {
                $$ = $1;
            }
        | VOID
        ;

param-lista:    param-lista COMMA param
                {
                    YYSTYPE t = $1;
                    if (t != NULL)
                    {
                        t = findLastSibling(t);
                        t->sibling = $3;
                        $$ = $1;
                    }
                    else $$ = $3;
                }

                | param
                    {
                        $$=$1;
                    }
                ;

param:  tipo-especificador ID
            { 
                $$ = newStmtNode(VarDeclK);
                $$->attr.name = tokenString;
                $$->type = savedType;
            }

        | tipo-especificador ID 
            { 
                $$ = newStmtNode(VectorParamK);
                $$->attr.name = tokenString;
                $$->type = savedType;
            }
            LBRACE RBRACE
            {
                $$ = $3;
            }
        | error 
            {
                $$ = NULL;
            }
        ;

composto-decl:  LBRACKET local-declaracoes statement-lista RBRACKET
                    { 
                        $$ = newStmtNode(CompoundK);
                        $$->child[0] = $2;
                        $$->child[1] = $3;
                        
                    }
                ;

local-declaracoes:  local-declaracoes var-declaracao
                        {       
                            YYSTYPE t = $1;
                            if (t != NULL)
                            { 
                                t = findLastSibling(t);
                                t->sibling = $2;
                                $$ = $1; 
                            }
                            else $$ = $2;
                        }
                    | %empty
                        {
                            $$ = NULL;
                        }
                    ;

statement-lista:    statement-lista statement
                        {       
                            YYSTYPE t = $1;
                            if (t != NULL)
                            { 
                                t = findLastSibling(t);
                                t->sibling = $2;
                                $$ = $1; 
                            }
                            else $$ = $2;
                        }
                    | %empty
                        {
                            $$ = NULL;
                        }
                    ;

statement:  expressao-decl
            | composto-decl
            | selecao-decl
            | iteracao-decl
            | retorno-decl
            ;

expressao-decl: expressao SEMI
                | SEMI 
                    {
                        $$ = NULL;
                    }
                ;

selecao-decl:   IF LPAREN expressao RPAREN statement %prec "then"
                    {       
                        $$ = newStmtNode(IfK);
                        $$->attr.val = newif();
                        $$->child[0] = $3;
                        $$->child[1] = $5;
                    }
                | IF LPAREN expressao RPAREN statement ELSE statement
                    {       
                        $$ = newStmtNode(IfK);
                        $$->attr.val = newif();
                        $$->child[0] = $3;
                        $$->child[1] = $5;
                        $$->child[2] = $7;
                    }
                ;

iteracao-decl:  WHILE LPAREN expressao RPAREN statement
                    {       
                        $$ = newStmtNode(WhileK);
                        $$->attr.val = newWhile();
                        $$->child[0] = $3;
                        $$->child[1] = $5;
                    }
                ;

retorno-decl:
        RETURN SEMI
            {       
                $$ = newStmtNode(ReturnK);
            }
        | RETURN expressao SEMI
            {       
                $$ = newStmtNode(ReturnK);
                $$->child[0] = $2;
            }
        ;

expressao:  var EQATR expressao
                {       
                    $$ = newStmtNode(AssignK);
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                }
            | simples-expressao
            ;

fator:  LPAREN expressao RPAREN
            {
                $$=$2;
            }
        | var
        | ativacao
        | NUM
            {
                $$=newExpNode(ConstK);
                $$->attr.val=atoi(tokenString);
            }
        | error 
            {
                $$=NULL;
            }
        ;


var:    ID
            { 
                $$ = newExpNode(IdK);
                $$->attr.name = tokenString;
            }
        | ID
            {
                $$ = newExpNode(VetK);
                $$->attr.name = tokenString;
            }
            LBRACE expressao RBRACE
            { 
                $$ = $2;
                $$->child[0]=$4;
            }
        ;


simples-expressao:  soma-expressao relacional soma-expressao
                        { 
                            $$ = $2;
                            $$->child[0]=$1;
                            $$->child[1]=$3;
                        }

                    | soma-expressao
                    ;

relacional: LESSEQ
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

soma-expressao: soma-expressao soma termo
                { 
                    $$ = $2;
                    $$->child[0]=$1;
                    $$->child[1]=$3;
                }

                | termo
                ;

soma:   PLUS
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
        | MOD
            {
                $$ = newExpNode(OpK);
                $$->attr.op = MOD;
            }
        | LEFTSHIFT
            {
                $$ = newExpNode(OpK);
                $$->attr.op = LEFTSHIFT;
            }
        | RIGHTSHIFT
            {
                $$ = newExpNode(OpK);
                $$->attr.op = RIGHTSHIFT;
            }
        ;


ativacao:
        ID 
            {
                $$ = newStmtNode(FunActiveK);
                $$->attr.name= "fun_" + (std::string) tokenString;
            }
            LPAREN args RPAREN
            {       
                YYSTYPE argument = $4;
                int argCount = 0;
                while(argument != NULL){
                    argCount++;
                    argument = argument->sibling;
                }
                $$ = $2;
                $$->attr.val = argCount;
                $$->child[0] = $4;
            }
        ;

args:   arg-lista

        | %empty
            {
                $$ = NULL;
            }
        ;

arg-lista:  arg-lista COMMA expressao
                {       
                    $$ = $3;
                    $$->sibling = $1;
                }
            | expressao
            ;

%%

int yyerror(const char * message)
{ 
    fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
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




