#ifndef _SYNTATIC_ERRORS_
#define _SYNTATIC_ERRORS_
#include "globals.h"

void IntInvalidReturnError(TreeNode *t, char * scope);
void VoidInvalidReturnError(TreeNode *t, char * scope);
void funcAlreadyDefinedError(TreeNode *t);
void varAlreadyDefinedError(TreeNode *t);
void varNotDefinedError(TreeNode *t);
void funcNotDefinedError(TreeNode *t);
void DeclaredTypeNotVectorError(TreeNode *t);
void isNotFuncError(TreeNode *t);
void VoidVarError(TreeNode *t);
void VoidVecError(TreeNode *t);

#endif