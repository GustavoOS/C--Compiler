/****************************************************/
/* File: globals.h                                  */
/* Yacc/Bison Version                               */
/* Global types and vars for TINY compiler          */
/* must come before other include files             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Yacc/Bison generates internally its own values
 * for the tokens. Other files can access these values
 * by including the tab.h file generated using the
 * Yacc/Bison option -d ("generate header")
 *
 * The YYPARSER flag prevents inclusion of the tab.h
 * into the Yacc/Bison output itself
 */

#ifndef YYPARSER

/* the name of the following file may change */
#include "cminus.tab.h"

/* ENDFILE is implicitly defined by Yacc/Bison,
 * and not included in the tab.h file
 */
#define ENDFILE 0

#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 8

/* Yacc/Bison generates its own integer values
 * for tokens
 */

typedef int TokenType;

extern "C" FILE *source;  /* source code text file */
extern "C" FILE *listing; /* listing output text file */
extern "C" FILE *code;    /* code text file for TM simulator */

extern "C" int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum
{
  StmtK,
  ExpK
} NodeKind; //Bolinha ou quadrado
typedef enum
{
  IfK,
  WhileK,
  AssignK,
  ReturnK,
  VarDeclK,
  VetDeclK,
  FunDeclK,
  FunActiveK,
  CompoundK,
  VectorParamK
} StmtKind;
typedef enum
{
  OpK,
  ConstK,
  IdK,
  VetK
} ExpKind;

/* ExpType is used for type checking */
typedef enum
{
  Void,
  Integer,
  boolean
} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode
{
  struct treeNode *child[MAXCHILDREN];
  struct treeNode *sibling;
  int lineno;
  NodeKind nodekind;
  union {
    StmtKind stmt;
    ExpKind exp;
  } kind;
  union {
    TokenType op; //Token of logic or arithmetic operator
    int val;      // Value of a constant
    char *name;   //Name of a function or variable (ID) ( int peanutbutter )
  } attr;
  ExpType type; /* for type checking of expressions */
} TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern "C" int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern "C" int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern "C" int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern "C" int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern "C" int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */
extern "C" int Error;
#endif
