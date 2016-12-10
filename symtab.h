

/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
#include "globals.h"

#ifndef _SYMTAB_H_
#define _SYMTAB_H_
/* the list of line numbers of the source 
 * code in which a variable is referenced
 */

typedef enum {VARIABLE, VECTOR, FUNCTION} IDType;

typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } * LineList;

/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
typedef struct BucketListRec
   { char * name;
     LineList lines;
     int memloc ; /* memory location for variable */
     struct BucketListRec * next;
     char * scope;
     IDType vtype;
     ExpType dtype;
   } * BucketList;

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
BucketList st_insert( char * name, int lineno, int loc, IDType type  );

/* Function st_lookup returns the memory reference 
   to the bucketlist of the variable or NULL if not found
 */
BucketList st_find(char * name);

/* Function st_lookup returns the memory location 
   of the variable or -1 if not found
 */
int st_lookup ( char * name );

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing);

#endif
