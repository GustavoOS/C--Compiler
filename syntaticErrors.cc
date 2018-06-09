#include "globals.h"
#include "syntaticErrors.h"

void IntInvalidReturnError(TreeNode *t, char * scope)
{
    printf(
        "\nError: Function \" %s \" is an int one, yet it is not returning anything at line %d.\n", scope, t->lineno);
    Error = TRUE;
}

void VoidInvalidReturnError(TreeNode *t, char * scope)
{
    printf(
        "\nError: Function \" %s \" is a void one, yet it is returning something at line %d.\n", scope, t->lineno);
    Error = TRUE;
}

void funcAlreadyDefinedError(TreeNode *t)
{
    printf(
        "\nError: Function \" %s \" already defined at line %d.\n", t->attr.name, t->lineno);
    Error = TRUE;
}

void varAlreadyDefinedError(TreeNode *t)
{
    printf(
        "\nError: Variable \" %s \" already defined at line %d.\n", t->attr.name, t->lineno);
    Error = TRUE;
}

void varNotDefinedError(TreeNode *t)
{
    printf(
        "\nError: Variable \" %s \" not defined at line %d.\n", t->attr.name, t->lineno);
    Error = TRUE;
}

void funcNotDefinedError(TreeNode *t)
{
    printf(
        "\nError: Function \" %s \" not defined at line %d.\n", t->attr.name, t->lineno);
    Error = TRUE;
}

void DeclaredTypeNotVectorError(TreeNode *t)
{
    printf(
        "\nError: \" %s \" is not a vector at line %d.\n", t->attr.name, t->lineno);
    Error = TRUE;
}

void isNotFuncError(TreeNode *t)
{
    printf(
        "\nError: \" %s \" is not a function at line %d.\n", t->attr.name, t->lineno);
    Error = TRUE;
}

void VoidVarError(TreeNode *t)
{
    printf(
        "\nError: \" %s \" variable has a forbidden type at line %d.\n", t->attr.name, t->lineno);
    Error = TRUE;
}

void VoidVecError(TreeNode *t)
{
    printf(
        "\nError: \" %s \" vector has a forbidden type at line %d.\n", t->attr.name, t->lineno);
    Error = TRUE;
}

