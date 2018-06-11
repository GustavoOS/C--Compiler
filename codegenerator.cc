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
        std::cout << "Location: " << getRecordFromSymbleTable(node)->memloc << "\n";
        //TODO GENERATE CODE
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

class TypeAInstruction : public Instruction
{
    std::string to_string()
    {
        return "(" + std::to_string(id) + ") " +
               name + " " + std::to_string(immediate) + " " +
               std::to_string(regm) + " " + std::to_string(regd) + "\n";
    }
};

class TypeBInstruction : public Instruction
{
    std::string to_string()
    {
        return "(" + std::to_string(id) + ") " +
               name + " " + std::to_string(regm) + " " +
               std::to_string(regn) + " " + std::to_string(regd) + "\n";
    }
};

class TypeCInstruction : public Instruction
{
    std::string to_string()
    {
        return "(" + std::to_string(id) + ") " +
               name + " " + std::to_string(immediate) + " " +
               std::to_string(regm) + " " + std::to_string(regd) + "\n";
    }
};

class TypeDInstruction : public Instruction
{
    std::string to_string()
    {
        return "(" + std::to_string(id) + ") " +
               name + " " + std::to_string(regd) + " " +
               std::to_string(immediate) + "\n";
    }
};

class TypeEInstruction : public Instruction
{
    std::string to_string()
    {
        return "(" + std::to_string(id) + ") " +
               name + " " + std::to_string(regm) + " " +
               std::to_string(regd) + "\n";
    }
};

class TypeFInstruction : public Instruction
{
    std::string to_string()
    {
        return "(" + std::to_string(id) + ") " +
               name + " " + std::to_string(condition) + " " +
               std::to_string(regd) + "\n";
    }
};

class TypeGInstruction : public Instruction
{
    std::string to_string()
    {
        return "(" + std::to_string(id) + ") " +
               name + " " + std::to_string(condition) + " " +
               std::to_string(offset) + " :" + label + "\n";
    }
};

class LabelDefinition: public Instruction
{
    std::string to_string(){
        return label + ":\n";
    }
};