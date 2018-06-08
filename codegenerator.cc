#include "codegenerator.h"
#include "globals.h"
#include "util.h"
#include <iostream>

CodeGenerator::CodeGenerator(bool displayable)
{
    shouldPrintGeneratedCodeOnScreen = displayable;

    //Registers
    HeapArrayRegister = 1;
    AcumulatorRegister = 2;
}

void CodeGenerator::print(std::string code)
{
    if (shouldPrintGeneratedCodeOnScreen)
    {
        std::cout << code << "\n";
    }
    generatedCode += code;
}

void CodeGenerator::generate(TreeNode *node)
{
    std::cout << "generateFunction\n";
    if (node == NULL)
        return;
    printNode(node);
    if (node->nodekind == StmtK)
        generateCodeForStmtNode(node);
    else
        generateCodeForExprNode(node);
}

void CodeGenerator::generateCodeForStmtNode(TreeNode *node)
{
    std::cout << "This is a statement\n";
    switch (node->kind.stmt)
    {
    case CompoundK:

        generate(node->child[0]);
        generate(node->child[1]);
        break;
    case VetDeclK:
        std::cout << node->child[0]->attr.val << "\n";
        break;

    default:
        break;
    }
}

void CodeGenerator::generateCodeForExprNode(TreeNode *node)
{
    std::cout << "This is an expression\n";
}