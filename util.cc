/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/* Adapted by Gustavo O. Souza                      */
/****************************************************/

#include "globals.h"
#include "util.h"
#include <iostream>

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */

void printToken(TokenType token, const char *tokenString)
{
  switch (token)
  {
  case ERROR:
    fprintf(listing,
            "ERROR: %s\n", tokenString);
    break;
  case ELSE:
  case IF:
  case INT:
  case RETURN:
  case VOID:
  case WHILE:
    fprintf(listing,
            "reserved word: %s\n", tokenString);
    break;
  case ID:
    fprintf(listing,
            "ID, name= %s\n", tokenString);
    break;
  case NUM:
    fprintf(listing,
            "NUM, val= %s\n", tokenString);
    break;
  case PLUS:
    fprintf(listing, "+\n");
    break;
  case MINUS:
    fprintf(listing, "-\n");
    break;
  case TIMES:
    fprintf(listing, "*\n");
    break;
  case SLASH:
    fprintf(listing, "/\n");
    break;

  case LESSER:
    fprintf(listing, "<\n");
    break;
  case LESSEQ:
    fprintf(listing, "<=\n");
    break;
  case GREATER:
    fprintf(listing, ">\n");
    break;
  case GREATEQ:
    fprintf(listing, ">=\n");
    break;
  case EQCOMP:
    fprintf(listing, "==\n");
    break;
  case NOTEQ:
    fprintf(listing, "!=\n");
    break;
  case EQATR:
    fprintf(listing, "=\n");
    break;

  case SEMI:
    fprintf(listing, ";\n");
    break;
  case COMMA:
    fprintf(listing, ",\n");
    break;

  case LPAREN:
    fprintf(listing, "(\n");
    break;
  case RPAREN:
    fprintf(listing, ")\n");
    break;
  case LBRACKET:
    fprintf(listing, "[\n");
    break;
  case RBRACKET:
    fprintf(listing, "]\n");
    break;
  case LBRACE:
    fprintf(listing, "{\n");
    break;
  case RBRACE:
    fprintf(listing, "}\n");
    break;

  case ENDFILE:
    fprintf(listing, "EOF\n");
    break;
  default: /* should never happen */
    fprintf(listing, "Unknown token: %d\n", token);
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode *newStmtNode(StmtKind kind)
{
  TreeNode *t = new TreeNode;
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode *newExpNode(ExpKind kind)
{
  TreeNode *t = new TreeNode;
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char *copyString(char *s)
{
  int n;
  char *result;
  if (s == NULL)
    return NULL;
  n = strlen(s) + 1;
  result = (char *)malloc(n * sizeof(char));
  if (result == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
    strcpy(result, s);
  return result;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno += 2
#define UNINDENT indentno -= 2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
  int i;
  for (i = 0; i < indentno; i++)
    fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode *tree)
{
  int i;
  INDENT;
  while (tree != NULL)
  {
    printSpaces();

    printNode(tree);
    for (i = 0; i < MAXCHILDREN; i++)
      printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}

void printNode(TreeNode *tree)
{
  if (tree->nodekind == StmtK)
  {
    switch (tree->kind.stmt)
    {
    case IfK:
      std::cout << "If\n";
      break;
    case WhileK:
      std::cout << "While\n";
      break;
    case AssignK:
      std::cout << "Assignment\n";
      break;
    case ReturnK:
      std::cout << "Return\n";
      break;
    case VarDeclK:
      std::cout << "Variable declaration: "
                << tree->attr.name << "\n";
      break;
    case VetDeclK:
      std::cout << "Vector declaration: "
                << tree->attr.name << "\n";
      break;
    case VectorParamK:
      std::cout << "Vector parameter: "
                << tree->attr.name << "\n";
      break;

    case FunDeclK:
      std::cout << "Function declaration: "
                << tree->attr.name << "\n";
      break;
    case FunActiveK:
      std::cout << "Activation of: "
                << tree->attr.name << "\n";
      break;
    case CompoundK:
      std::cout << "Compound\n";
      break;
    default:
      std::cout << "Unknown ExpNode kind\n";
      break;
    }
  }
  else if (tree->nodekind == ExpK)
  {
    switch (tree->kind.exp)
    {
    case OpK:
      std::cout << "Op: ";
      printToken(tree->attr.op, "\0");
      break;
    case ConstK:
      std::cout << "Const: "
                << tree->attr.val << "\n";
      break;
    case VetK:
      std::cout << "Vet id.: "
                << tree->attr.name << "\n";
      break;
    case IdK:
      std::cout << "Id: "
                << tree->attr.name << "\n";
      break;
    default:
      std::cout << "Unknown ExpNode kind\n";
      break;
    }
  }
  else
    std::cout << "Unknown node kind\n";
}
