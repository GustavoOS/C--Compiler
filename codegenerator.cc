#include "codegenerator.h"
#include "globals.h"
#include "util.h"
#include "symtab.h"
#include <iostream>

int translateCondition(int operation)
{

    switch (operation)
    {
    case LESSER:
        return 11;
    case LESSEQ:
        return 9;
    case GREATER:
        return 12;
    case GREATEQ:
        return 10;
    case EQCOMP:
        return 0;
    case NOTEQ:
        return 1;
    default:
        return 14;
    }
}

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
            6,
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
        print(new TypeDInstruction(
            10,
            "ADD",
            node->attr.val * 4,
            HeapArrayRegister));
        generateCode(node->sibling);
        break;
    case VarDeclK:
        std::cout << "Variable " << node->attr.name << "\n";
        generateCode(node->sibling);
        break;
    case IfK:
        generateCode(node->child[0]);
        print(
            new BranchLabel(
                "if_true_" + std::to_string(node->attr.val),
                node->child[0]->attr.op));

        if (node->child[2] != NULL)
            generateCode(node->child[2]);

        print(
            new BranchLabel(
                "if_end_" + std::to_string(node->attr.val),
                -1 //Always
                ));
            print(nopWithLabel("if_true_"+ std::to_string(node->attr.val)));
            generateCode(node->child[1]);

        print(nopWithLabel("if_end_" + std::to_string(node->attr.val)));

        break;

    default:
        std::cout << "Code not generated for this node\n";
        break;
    }
}

void CodeGenerator::generateCodeForExprNode(TreeNode *node)
{
    std::cout << "This is an expression\n";

    switch (node->kind.stmt)
    {
    case OpK:
    {
        generateCode(node->child[0]);
        print(
            pushAcumulator());
        generateCode(node->child[1]);
        print(
            new TypeEInstruction(
                68,
                "POP",
                0,
                TemporaryRegister));
        generateOperationCode(node);
    }
    break;

    default:
        break;
    }
}
void CodeGenerator::generateOperationCode(TreeNode *node)
{

    switch (node->attr.op)
    {
    case PLUS:
        print(
            new TypeBInstruction(
                4,
                "ADD",
                AcumulatorRegister,
                TemporaryRegister,
                AcumulatorRegister));
        break;
    case MINUS:
        print(
            new TypeBInstruction(
                5,
                "SUB",
                AcumulatorRegister,
                TemporaryRegister,
                AcumulatorRegister));
        break;
    case TIMES:
        print(
            new TypeEInstruction(
                25,
                "MUL",
                TemporaryRegister,
                AcumulatorRegister));
        break;

    case SLASH:
        print(new TypeCInstruction(
            6,
            "ADD",
            0,
            AcumulatorRegister,
            SwapRegister));
        print(
            new TypeCInstruction(
                6,
                "ADD",
                0,
                TemporaryRegister,
                AcumulatorRegister));
        print(
            new TypeCInstruction(
                6,
                "ADD",
                0,
                SwapRegister,
                TemporaryRegister));
        print(
            new TypeEInstruction(
                34,
                "DIV",
                TemporaryRegister,
                AcumulatorRegister));
        break;

    default:
        print(
            new TypeEInstruction(
                22,
                "CMP",
                TemporaryRegister,
                AcumulatorRegister));

        break;
    }
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

Instruction *pushAcumulator()
{
    return new TypeEInstruction(
        67,
        "PUSH",
        0,
        AcumulatorRegister);
}

Instruction *nopWithLabel(std::string label)
{
    Instruction *temp = new TypeDInstruction(74, "NOP", 0, 0);
    temp->setlabel(label);
    return temp;
}