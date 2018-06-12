#include "codegenerator.h"
#include "globals.h"
#include "util.h"
#include "symtab.h"
#include <iostream>

//Code Generator Class
CodeGenerator::CodeGenerator(bool displayable)
{
    shouldPrintGeneratedCodeOnScreen = displayable;
}

void CodeGenerator::print(Instruction *instruction)
{
    std::string inst = instruction->to_string_with_label();
    if (shouldPrintGeneratedCodeOnScreen)
    {
        std::cout << inst;
    }
    generatedCode += inst;
    code.push_back(*instruction);
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
    printNode(node); //Check visited node
    if (node->nodekind == StmtK)
        generateCodeForStmtNode(node);
    else
        generateCodeForExprNode(node);
}

BucketList getRecordFromSymbleTable(TreeNode *node)
{
    return st_find_at_scope(node->attr.name, (char *)node->scope.c_str());
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
        // std::cout << "Vector size: " << node->attr.val << "\n";
        // std::cout << "Location: " << getRecordFromSymbleTable(node)->memloc << "\n";
        //TODO GENERATE CODE
        print(new TypeCInstruction(
            10,
            "ADD",
            0,
            HeapArrayRegister,
            AcumulatorRegister));
        print(new TypeAInstruction(
            48, 
            "STR", 
            getRecordFromSymbleTable(node)->memloc + 1,
            AcumulatorRegister,
            AcumulatorRegister));
        print(new TypeAInstruction(
            6,
            "ADD",
            node->attr.val * 4,
            HeapArrayRegister,
            HeapArrayRegister));
        generateCode(node->sibling);
        break;
    case VarDeclK:
        std::cout << "Variable" << node->attr.name << "\n";
        generateCode(node->sibling);
        break;

    default:
        std::cout << "Code not generated for this node\n";
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

//Instruction Class
std::string Instruction::to_string()
{
    return "Generic Instruction";
}
