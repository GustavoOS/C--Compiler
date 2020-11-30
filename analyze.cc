/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/* Adapted by Gustavo O. Souza                      */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include "memory.h"
#include "syntaticErrors.h"
#include <string>

//current scope, standard is global
std::string scope = (std::string) "global";

bool hasAllocations = false;

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
void traverse(TreeNode *t,
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
void nullProc(TreeNode *t)
{
  return;
}

static void exitScope(TreeNode *t)
{
  if (t->nodekind == StmtK && t->kind.stmt == FunDeclK)
  {
    // printf("Sai do escopo %s\n", scope);
    scope = std::string("global");
  }
}

int isTreeNodeTypeInt(TreeNode *node)
{
  return (node != NULL && node->type == Integer);
}

// int notFoundInScope( BucketList l )
// {
//   return (l == NULL) || (l != NULL && (strcmp(scope, l->scope) != 0));
// }

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertNode(TreeNode *t)
{
  DataSection *dataSection = new DataSection();
  BucketList l;
  std::string dvariable;

  switch (t->nodekind)
  {
  case StmtK:
    switch (t->kind.stmt)
    {
    case VarDeclK:
      l = st_find(t->attr.name, scope);

      if (l == NULL)
      {
        if (isTreeNodeTypeInt(t))
        {
          st_declare(t->attr.name, t->lineno, dataSection->allocateVariable(scope), VARIABLE, scope);
          t->scope = (std::string)scope;
        }
        else
        {
          VoidVarError(t);
        }
      }
      else
      {
        varAlreadyDefinedError(t);
      }

      break;

    case VetDeclK:
      
      dvariable = scope;
      l = st_find(t->attr.name, scope);
      if ((l == NULL) || (scope!= l->scope))
      {
        if (isTreeNodeTypeInt(t))
        {
          hasAllocations = hasAllocations || (t->attr.val > 0);
          st_declare(t->attr.name,
                     t->lineno,
                     dataSection->allocateVariable(
                         scope),
                     VECTOR,
                     scope);
          t->scope = (std::string)scope;
        }
        else
        {
          VoidVecError(t);
        }
      }
      else
      {
        varAlreadyDefinedError(t);
      }
      break;

    case VectorParamK:
      dvariable = scope;
      l = st_find(t->attr.name, scope);
      if ((l == NULL) || (scope != l->scope) )
      {
        if (isTreeNodeTypeInt(t))
        {
          st_declare(
              t->attr.name,
              t->lineno,
              dataSection->allocateVariable(scope),
              VECTOR,
              scope);
        }
        else
        {
          VoidVecError(t);
        }
      }
      else
      {
        varAlreadyDefinedError(t);
      }
      break;

    case FunDeclK:
      l = st_find(t->attr.name, scope);
      if ((l == NULL) ||  (scope != l->scope))
      {
        BucketList l2 = st_declare_function(t->attr.name, t->lineno, t->lineno > 0 ? dataSection->allocateFunction() : 0, FUNCTION, t->type, scope);
        l2->dtype = t->type;
        scope = t->attr.name;
        // printf("\nAnalyze 168: Entrei no escopo %s", scope);
      }
      else
      {
        funcAlreadyDefinedError(t);
      }
      break;

    case FunActiveK:
    {
      l = st_find(t->attr.name, scope);
      if (l == NULL)
      {
        funcNotDefinedError(t);
      }
      else
      {
        if (l->vtype == FUNCTION)
        {
          st_reference(l, t->lineno);
        }
        else
        {
          isNotFuncError(t);
        }
      }
      break;
    }

    case ReturnK:
    {
      l = st_find( scope, "global");
      t->scope = scope;
      switch (l->dtype)
      {
      case Void:
      {
        if (t->child[0] != NULL)
        {
          VoidInvalidReturnError(t, scope);
        }

        break;
      }
      case Integer:
      {
        if (t->child[0] == NULL)
        {
          IntInvalidReturnError(t,  scope);
        }

        break;
      }
      default:
        printf("Vishe! Nem eu entendi");
        break;
      }
      break;
    }

    default:
      break;
    }
    break;

  case ExpK:
    switch (t->kind.exp)
    {
    case VetK:
    {
      l = st_find(t->attr.name, scope);
      if (l == NULL)
      {
        varNotDefinedError(t);
      }
      else
      {

        if (l->vtype == VECTOR)
        {
          BucketList ref;
          t->type = l->dtype;
          ref = st_reference(l, t->lineno);
          t->scope = ref->scope;
        }
        else
        {
          DeclaredTypeNotVectorError(t);
        }
      }
    }
    break;

    case IdK:
    {
      l = st_find(t->attr.name, scope);
      if (l == NULL)
      {
        varNotDefinedError(t);
      }
      else
      {
        BucketList ref;
        t->type = l->dtype;
        ref = st_reference(l, t->lineno);
        t->scope = ref->scope;
      }
    }
    break;

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
  BucketList myList = st_find("fun_main", scope);
  if (myList == NULL)
  {
    Error = TRUE;
    printf("\nError: Main function was not found.\n");
  }
  else
  {
    if (myList->vtype != FUNCTION)
    {
      printf("\nError: Main is not a function.\n");
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

static void typeError(TreeNode *t, std::string message)
{
  fprintf(listing, "Type error at line %d: %s\n", t->lineno, message.c_str());
  Error = TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode *t)
{
  switch (t->nodekind)
  {
  case ExpK:
    switch (t->kind.exp)
    {
    case OpK:
      if ((t->child[0]->type != Integer) ||
          (t->child[1]->type != Integer))
        typeError(t, "Op applied to non-integer");
      switch (t->attr.op)
      {
      case LESSEQ:
      case LESSER:
      case GREATER:
      case GREATEQ:
      case EQCOMP:
      case NOTEQ:
        t->type = boolean;
        break;
      case PLUS:
      case MINUS:
      case TIMES:
      case SLASH:
      case MOD:
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
      if (t->child[0] == NULL)
      {
        typeError(t->child[0], "if test invalid");
      }
      else if (t->child[0]->type != boolean)
        typeError(t->child[0], "if test is not boolean");
      break;
    case AssignK:
      if (t->child[1]->type != Integer)
        typeError(t->child[1], "assignment of non-integer value");
      break;
    case WhileK:
      if (t->child[0]->type != boolean)
        typeError(t->child[0], "while test is not boolean");
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
