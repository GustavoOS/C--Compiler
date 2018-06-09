#include "codegenerator.h"
#include "globals.h"
#include "util.h"
#include "symtab.h"
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
    createHeader();
    generateCode(node);
    createFooter();
}

void CodeGenerator::generateCode(TreeNode *node)
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
        generateCode(node->child[0]);
        generateCode(node->child[1]);
        break;

    case VetDeclK:
        std::cout << "Vector size: "<< node->child[0]->attr.val << "\n";
        std::cout << "Location: " << st_find_at_scope(node->attr.name, (char *) node->scope.c_str())->memloc << "\n";
        //TODO GENERATE CODE
        generateCode(node->sibling);
        break; 

    default:
        break;
    }
}

void CodeGenerator::generateCodeForExprNode(TreeNode *node)
{
    std::cout << "This is an expression\n";
}

//Headers and Footers

void CodeGenerator::createHeader()
{
    std::cout << "This is a header\n";
}

void CodeGenerator::createFooter()
{
    std::cout << "This is a footer\n";
}