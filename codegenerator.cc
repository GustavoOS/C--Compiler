#include "codegenerator.h"
#include "globals.h"
#include "util.h"
#include "symtab.h"
#include "memory.h"
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

void CodeGenerator::generateCodeForAnyNode(TreeNode *node)
{
    if (node == NULL)
        return;
    if (node->nodekind == StmtK)
        generateCodeForStmtNode(node);
    else
        generateCodeForExprNode(node);
}

void CodeGenerator::generateCode(TreeNode *node)
{
    std::cout << "generateFunction\n";
    if (node == NULL){
        std::cout << "This node is NULL, exiting\n";
        return;}
    printNode(node); //Check visited node
    generateCodeForAnyNode(node);
    std::cout << "generateCode visiting brother\n";
    generateCode(node->sibling);
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

        break;

    case VarDeclK:
        std::cout << "Variable " << node->attr.name << "\n";

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
        print(nopWithLabel("if_true_" + std::to_string(node->attr.val)));
        generateCode(node->child[1]);

        print(nopWithLabel("if_end_" + std::to_string(node->attr.val)));

        break;

    case FunDeclK:
    {
        std::string FunctionName = std::string(node->attr.name);
        generateCode(node->child[0]);
        generateCode(node->child[1]);
        if (FunctionName != "input" && FunctionName != "output" && FunctionName != "outputLED")
            std::cout << "Missing Implementation to function declaration\n";
    }
    break;

    case FunActiveK:
    {
        std::string FunctionName = std::string(node->attr.name);
        generateCode(node->child[0]);
        if (FunctionName == "input")
        {
            print(
                new TypeEInstruction(
                    71,
                    "INSW",
                    0,
                    AcumulatorRegister));
        }
        else if (FunctionName == "output")
        {
            print(
                new TypeEInstruction(
                    69,
                    "OUTSS",
                    0,
                    AcumulatorRegister));
        }
        else
        {

            if (FunctionName == "outputLED")
            {
                print(
                    new TypeEInstruction(
                        70,
                        "OUTLED",
                        0,
                        AcumulatorRegister));
            }
            else
            {
                DataSection ds;
                int variableCountInFunction = ds.getSize(node->attr.name);
                int argumentCount = node->attr.val;
                print(
                    pushRegister(FramePointer));
                //Build activation record
                print(
                    new TypeDInstruction(
                        8,
                        "MOV",
                        AcumulatorRegister,
                        0));
                for (
                    int i = 0;
                    i < variableCountInFunction - argumentCount;
                    i++)
                    print(pushAcumulator());
                TreeNode *argumentNode = node->child[0];
                for (int i = 0; i < argumentCount; i++)
                {
                    generateCodeForAnyNode(argumentNode);
                    print(pushAcumulator());
                }
                //All variables pushed
                /*TODO save PC count into RA*/
                /*TODO branch to function definition*/

                std::cout << "Implementation to function activation is a work in progress\n";
            }
        }
    }
    break;

    default:
        std::cout
            << "Code not generated for this node\n";
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

Instruction *pushRegister(int reg)
{
    return new TypeEInstruction(
        67,
        "PUSH",
        0,
        reg);
}
Instruction *pushAcumulator()
{
    return pushRegister(AcumulatorRegister);
}

Instruction *nopWithLabel(std::string label)
{
    Instruction *temp = new TypeDInstruction(74, "NOP", 0, 0);
    temp->setlabel(label);
    return temp;
}