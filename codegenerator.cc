#include "codegenerator.h"
#include "globals.h"
#include <iostream>

CodeGenerator::CodeGenerator(bool displayable)
{
    shouldPrintGeneratedCodeOnScreen = displayable;
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
    if (node == NULL)
        return;
    if (node->nodekind == StmtK)
        generateCodeForStmtNode(node);
    else
        generateCodeForExprNode(node);
}

void CodeGenerator::generateCodeForStmtNode(TreeNode * node){
    std::cout << "This is a statement\n";
}

void CodeGenerator::generateCodeForExprNode(TreeNode *node)
{
    std::cout << "This is an expression\n";
}