/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* counter for variable memory locations */
static int location = 0;



/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse(TreeNode *t,
                     void (*preProc)(TreeNode *),
                     void (*postProc)(TreeNode *))
{
  if (t != NULL)
  {
    preProc(t);
    {
      int i;
      for (i = 0; i < MAXCHILDREN; i++)
        traverse(t->child[i], preProc, postProc);
    }
    postProc(t);
    traverse(t->sibling, preProc, postProc);
  }
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode *t)
{
  if (t == NULL||t!=NULL)
    return;
}

static void funcAlreadyDefinedError(TreeNode *t)
{
  printf(
    "Function %s already defined error at line %d.\n", t->attr.name, t->lineno);
  Error = TRUE;
}

static void varAlreadyDefinedError(TreeNode *t)
{
  printf(
    "Variable %s already defined error at line %d.\n", t->attr.name, t->lineno);
  Error = TRUE;
}

static void varNotDefinedError(TreeNode *t)
{
  printf(
    "Variable %s not defined error at line %d.\n", t->attr.name, t->lineno);
  Error = TRUE;
}

static void funcNotDefinedError(TreeNode *t)
{
  printf(
    "Function %s not defined error at line %d.\n", t->attr.name, t->lineno);
  Error = TRUE;
}

static void isNotVectorError(TreeNode *t)
{
  printf(
    "Error: %s is not a vector at line %d.\n", t->attr.name, t->lineno);
  Error = TRUE;
}

static void isNotVarError(TreeNode *t)
{
  printf(
    "Error: %s is not a variable at line %d.\n", t->attr.name, t->lineno);
  Error = TRUE;
}

static void isNotFuncError(TreeNode *t)
{
  printf(
    "Error: %s is not a function at line %d.\n", t->attr.name, t->lineno);
  Error = TRUE;
}

char scope[42] = "global";
static void exitScope( TreeNode *t )
{
  if(t->nodekind == StmtK && t->kind.stmt == FunDeclK ) {
    printf("Sai do escopo %s\n", scope);
    strcpy(scope, "global");
  }
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertNode(TreeNode *t)
{
  switch (t->nodekind)
  {
  case StmtK:
    switch (t->kind.stmt)
    {
    case VarDeclK:
      if (st_lookup(t->attr.name, scope) == -1){
        st_insert(t->attr.name, t->lineno, location++, VARIABLE, scope);
      }else{
        varAlreadyDefinedError(t);
      }
      break;
    case VetDeclK:
      if (st_lookup(t->attr.name, scope) == -1){
        st_insert(t->attr.name, t->lineno, location++, VECTOR, scope);
      }else{
        varAlreadyDefinedError(t);
      }
     break;
    case FunDeclK:
      if (st_lookup(t->attr.name, scope) == -1){
        BucketList l = st_insert(t->attr.name, t->lineno, location++, FUNCTION, scope);
        l->dtype = t->type;
        strcpy(scope, t->attr.name);
        printf("Entrei no escopo %s\n", scope);
      }else{
        funcAlreadyDefinedError(t);
      }
      break;
    case FunActiveK:
    {
      BucketList l = st_find(t->attr.name, scope);
      if (l == NULL){
        funcNotDefinedError(t);
      }else{
        if(l->vtype == FUNCTION){
          st_insert(t->attr.name, t->lineno, 0, FUNCTION, scope);
        }else{
          isNotFuncError(t);
        }
      }
    }
    break;
    default:
      break;
    }
    break;
  case ExpK:
    switch (t->kind.exp)
    {
    case VetK:
    {
      BucketList l = st_find(t->attr.name, scope);
       if (l == NULL){
        varNotDefinedError(t);
      }else{
        if(l->vtype == VECTOR){
          st_insert(t->attr.name, t->lineno, 0, VECTOR, scope);
        }else{
          isNotVectorError(t);
        }
      }
      break;
    }
    case IdK:
    {
      BucketList l = st_find(t->attr.name, scope);
      if (l == NULL){
        varNotDefinedError(t);
      }else{
        if(l->vtype == VARIABLE){
          st_insert(t->attr.name, t->lineno, 0, VARIABLE, scope);
        }else{
          isNotVarError(t);
        }
      }
      break;
    }
    default:
      break;
    }
    break;
  default:
    break;
  }
}

//
void mainVerify()
{
  BucketList myList = st_find("main",scope);
  if (myList == NULL)
  {
     Error = TRUE;
     printf("Error: Main function was not found.\n");
  }else{
     if( myList->vtype != FUNCTION){
       printf("Error: Main is not a function.\n");
     }
  }
}
//

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode *syntaxTree)
{
  traverse(syntaxTree, insertNode, exitScope);
  if (TraceAnalyze)
  {
    fprintf(listing, "\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

static void typeError(TreeNode *t, char *message)
{
  fprintf(listing, "Type error at line %d: %s\n", t->lineno, message);
  Error = TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode *t) {
  switch (t->nodekind)
  {
  case ExpK:
    switch (t->kind.exp)
    {
    case OpK:
      if ((t->child[0]->type != Integer) ||
          (t->child[1]->type != Integer))
        typeError(t, "Op applied to non-integer");
        switch( t->attr.op){
          case LESSEQ:
          case LESSER:
          case GREATER:
          case GREATEQ:
          case EQCOMP:
          case NOTEQ:
            t->type = Boolean;
            break;
          case PLUS:
          case MINUS:
          case TIMES:
          case SLASH:
            t->type = Integer;
            break;
          default:
            printf("ERROR! Unknown operator.");
        }
        break;
    case ConstK:
    case IdK:
      t->type = Integer;
      break;
    default:
      break;
    }
    break;
  case StmtK:
    switch (t->kind.stmt)
    {
    case FunActiveK:
      t->type = st_find(t->attr.name, scope)->dtype;
      break;
    case IfK:
      if(t->child[0] == NULL){
        typeError(t->child[0], "if test invalid");
      }else if (t->child[0]->type != Boolean)
        typeError(t->child[0], "if test is not Boolean");
      break;
    case AssignK:
      if (t->child[1]->type != Integer)
        typeError(t->child[1], "assignment of non-integer value");
      break;
    case WhileK:
      if (t->child[0]->type != Boolean)
        typeError(t->child[0], "while test is not Boolean");
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode *syntaxTree)
{
  traverse(syntaxTree, nullProc, checkNode);
}
