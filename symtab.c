/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4


/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}


/* the hash table */
static BucketList hashTable[SIZE];

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */

BucketList st_declare( char * name, int lineno, int loc, IDType type, char * scope)
{
    // printf( "%s name %d lineno %d loc %d type %s scope\n",name,  lineno,  loc,  type,  scope);
    int h = hash(name);  
    BucketList l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
    strcpy(l->scope,scope);
    l->next = hashTable[h];
    l->vtype = type;
    l->dtype = Integer;
    hashTable[h] = l; 
    return l;
 }

BucketList st_reference( BucketList l, int lineno)
{ 
    LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  return l;
} /* st_insert */



/* Function st_find searches a variable in local scope and then in global scope. Not finding anything makes it return NULL
 */
BucketList st_find ( char * name, char * escopo )
{ int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name)!= 0)&&((strcmp(escopo,l->scope)!=0)))
    l = l->next;
  if( (l == NULL) && (strcmp("global",escopo)!=0) ){
    l = hashTable[h];
    while((l != NULL) && ((strcmp("global",l->scope)!=0)))
       l = l->next;
  }
  return l;
}


/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ int i;
  fprintf(listing,"Variable Name  Data type  Location  Scope       Line Numbers\n");
  fprintf(listing,"-------------  ---------  --------  -----       ------------\n");
  for (i=0;i<SIZE;++i)
  { if (hashTable[i] != NULL)
    { BucketList l = hashTable[i];
      while (l != NULL)
      { LineList t = l->lines;
        fprintf(listing,"%-14s ",l->name);
        if( l->dtype == Integer ){
          fprintf(listing,"%-10s ","Integer");
        } else if( l->dtype == Boolean ){
          fprintf(listing,"%-10s ","Boolean");
        } else {
          fprintf(listing,"%-10s ","Void");
        }
        fprintf(listing,"%-8d  ",l->memloc);
        fprintf(listing,"%-10s  ",l->scope);
        while (t != NULL)
        { 
          fprintf(listing,"%4d ",t->lineno);
          t = t->next;
        }
        fprintf(listing,"\n");
        l = l->next;
      }
    }
  }
} /* printSymTab */
