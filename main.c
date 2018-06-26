/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/* Adapted by Gustavo O. Souza                      */
/****************************************************/
#include "globals.h"
#include <iostream>
#include "library.h"
/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE

/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE TRUE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !_CODE_GENERATOR_
#include "codegenerator.h"
#endif
#endif
#endif

/* allocate global variables */
int lineno;
FILE *originalSource;
FILE *source;
FILE *listing;
FILE *code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = TRUE;

int Error = FALSE;

int main(int argc, char *argv[])
{
  TreeNode *syntaxTree;
  char pgm[120]; /* source code file name */
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <filename>\n", argv[0]);
    exit(1);
  }
  strcpy(pgm, argv[1]);
  if (strchr(pgm, '.') == NULL)
    strcat(pgm, ".tny");
  originalSource = fopen(pgm, "r");
  if (originalSource == NULL)
  {
    fprintf(stderr, "File %s not found\n", pgm);
    exit(1);
  }
  fclose(originalSource);
  listing = stdout; /* send listing to screen */
  LibraryIncluder includer = LibraryIncluder(pgm);
  source = includer.getFinalFile();
  lineno = - includer.libSize;
  // lineno = 0;
  fprintf(listing, "\nC- COMPILATION: %s\n", pgm);
#if NO_PARSE
  while (getToken() != ENDFILE)
    ;
#else
  syntaxTree = parse();
  if (TraceParse)
  {
    fprintf(listing, "\nSyntax tree:\n");
    printTree(syntaxTree);
  }
#if !NO_ANALYZE
  if (!Error)
  {
    if (TraceAnalyze)
      fprintf(listing, "\nBuilding Symbol Table...\n");
    buildSymtab(syntaxTree);
  }
  if (!Error)
  {
    if (TraceAnalyze)
      fprintf(listing, "\nChecking for main Function...\n");
    mainVerify();
  }
  if (!Error)
  {
    if (TraceAnalyze)
      fprintf(listing, "\nChecking Types...\n");
    typeCheck(syntaxTree);
    if (TraceAnalyze)
      fprintf(listing, "\nType Checking Finished\n");
  }
  if (!Error)
  {
    CodeGenerator cg = CodeGenerator(TraceCode);
    std::cout << "\nStarting code generation process\n";
    std::cout << "--------------------------------------\n\n\n";
    cg.generate(syntaxTree);
  }
#if !NO_CODE
  if (!Error)
  {
    char *codefile;
    int fnlen = strcspn(pgm, ".");
    codefile = (char *)calloc(fnlen + 4, sizeof(char));
    strncpy(codefile, pgm, fnlen);
    strcat(codefile, ".tm");
    code = fopen(codefile, "w");
    if (code == NULL)
    {
      printf("Unable to open %s\n", codefile);
      exit(1);
    }
    codeGen(syntaxTree, codefile);
    fclose(code);
  }
#endif
#endif
#endif
  fclose(source);
  return 0;
}
