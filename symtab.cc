/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/* Adapted by Gustavo O. Souza                      */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */
static int hash(char *key)
{
  int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  {
    temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/* the hash table */
BucketList hashTable[SIZE];

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */

BucketList st_declare(char *name, int lineno, int loc, IDType type, std::string escopo)
{
  // printf( "%s name %d lineno %d loc %d type %s scope\n",name,  lineno,  loc,  type,  scope);
  int h = hash(name);
  BucketList l = (BucketList)malloc(sizeof(struct BucketListRec));
  l->name = name;
  l->lines = std::vector<int>();
  l->lines.push_back(lineno);
  l->memloc = loc;
  l->scope = escopo;
  l->next = hashTable[h];
  l->vtype = type;
  l->dtype = Integer;
  hashTable[h] = l;
  return l;
}

BucketList st_declare_function(char *name, int lineno, int loc, IDType type, ExpType eType, std::string escopo)
{
  // printf( "%s name %d lineno %d loc %d type %s scope\n",name,  lineno,  loc,  type,  scope);
  int h = hash(name);
  BucketList l = (BucketList)malloc(sizeof(struct BucketListRec));
  l->name = name;
  l->lines = std::vector<int>();
  l->lines.push_back(lineno);
  l->memloc = loc;
  l->scope = escopo;
  l->next = hashTable[h];
  l->vtype = type;
  l->dtype = eType;
  hashTable[h] = l;
  return l;
}

BucketList advanceNode(BucketList node)
{
  return (node != NULL) ? node->next : node;
}

BucketList st_reference(BucketList l, int lineno)
{
  l->lines.push_back(lineno);
  // LineList t = l->lines;
  // while (t->next != NULL)
  //   t = t->next;
  // t->next = (LineList)malloc(sizeof(struct LineListRec));
  // t->next->lineno = lineno;
  // t->next->next = NULL;
  return l;
} /* st_insert */

int cantMatchNameAndScopeInRange(BucketList node, char *name, std::string escopo)
{
  return (node != NULL) && (                                         //in range
                               (escopo != node->scope) || //Scopes don't match
                               (strcmp(name, node->name) != 0));     //names don't match
}

BucketList st_find_at_scope(char *name, std::string escopo)
{
  BucketList EntryNode = hashTable[hash(name)];
  while (cantMatchNameAndScopeInRange(EntryNode, name, escopo))
  {
    EntryNode = advanceNode(EntryNode);
  }
  return EntryNode;
}

/* Function st_find searches a variable in local scope and then in 
 * global scope.
 * Not finding anything makes it return NULL
 */
BucketList st_find(char *name, std::string escopo)
{
  BucketList result = st_find_at_scope(name, escopo);
  if (result == NULL)
    result = st_find_at_scope(name, "global");
  return result;
}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE *listing)
{
  int i;
  fprintf(listing, "Variable Name  Data type  Location  Scope       Line Numbers\n");
  fprintf(listing, "-------------  ---------  --------  -----       ------------\n");
  for (i = 0; i < SIZE; ++i)
  {
    if (hashTable[i] != NULL)
    {
      BucketList l = hashTable[i];
      while (l != NULL)
      {
        fprintf(listing, "%-14s ", l->name);
        if (l->dtype == Integer)
        {
          fprintf(listing, "%-10s ", "Integer");
        }
        else if (l->dtype == boolean)
        {
          fprintf(listing, "%-10s ", "boolean");
        }
        else
        {
          fprintf(listing, "%-10s ", "Void");
        }
        fprintf(listing, "%-8d  ", l->memloc);
        fprintf(listing, "%-10s  ", l->scope.c_str());
        for (int line : l->lines)
        {
          fprintf(listing, "%4d ", line);
        }
        fprintf(listing, "\n");
        l = l->next;
      }
    }
  }
} /* printSymTab */
