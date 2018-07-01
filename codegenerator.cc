#include "codegenerator.h"
#include "globals.h"
#include "util.h"
#include "symtab.h"
#include "memory.h"
#include <iostream>
#include <bitset>
#include <assert.h>
#include <fstream>

ConditionCodes translateCondition(TokenType operation)
{

    switch (operation)
    {
    case LESSER:
        return LT;
    case LESSEQ:
        return LE;
    case GREATER:
        return GT;
    case GREATEQ:
        return GE;
    case EQCOMP:
        return EQ;
    case NOTEQ:
        return NE;
    default:
        return AL;
    }
}

//Code Generator Class
CodeGenerator::CodeGenerator(bool displayable)
{
    shouldPrintGeneratedCodeOnScreen = displayable;
    shouldShowVisitingMessages = false;
    mainActivation = newStmtNode(FunActiveK);
    mainActivation->attr.name = "fun_main";
    mainActivation->type = Integer;
    mainActivation->attr.val = 0;
    mainActivation->child[0] = NULL;
    mainActivation->scope = "global";
}

void CodeGenerator::print(Instruction *instruction)
{
    std::string inst = instruction->to_string_with_label();
    if (shouldPrintGeneratedCodeOnScreen)
    {
        std::cout << inst << "\n";
    }
    generatedCode += inst;
    code.push_back(instruction);
}

void CodeGenerator::generate(TreeNode *node)
{
    createHeader();
    generateCode(node);
    createFooter();
}

void CodeGenerator::linker()
{
    for (auto item : labelDestMap)
    {
        std::string label = item.first;
        Instruction *label_dest = item.second;
        std::cout << label << " -> " << label_dest->name << "\n";
    }
    int i = 0;
    for (Instruction *inst : code)
    {
        inst->relativeAddress = i++;
    }

    for (auto item : labelOriginMap)
    {
        std::string label = item.first;
        // std::cout << "label" << label << "\n";
        BranchLabel *label_dest = item.second;
        int destinationAddress = labelDestMap[label]->relativeAddress;
        std::string fullNumber = std::bitset<16>(destinationAddress).to_string();
        std::string byteNumber = fullNumber.substr(0, 8);
        std::bitset<8> partialNumber(byteNumber);
        int byteAsInt = (int)partialNumber.to_ulong();
        label_dest->leftByte->immediate = byteAsInt;

        std::string byteNumber2 = fullNumber.substr(8, 8);
        std::bitset<8> partialNumber2(byteNumber2);
        int byteAsInt2 = (int)partialNumber2.to_ulong();

        label_dest->rightByte->immediate = byteAsInt2;
        std::cout << label << " -> " << label_dest->to_string() << "\n";

        std::cout << "destinationAddress: " << destinationAddress << "\n";
        std::cout << "left : " << label_dest->leftByte->to_string() << "\n";
        std::cout << "right: " << label_dest->rightByte->to_string() << "\n";
    }

    // printf("AFTER LINKER:");

    // for (Instruction *inst : code)
    // {
    //     std::cout << inst->relativeAddress << " : " << inst->to_string() << "\n";
    // }
}

void CodeGenerator::generateCodeForAnyNode(TreeNode *node)
{
    if (node == NULL)
    {
        if (shouldShowVisitingMessages)
            std::cout << "This node is NULL, exiting generateCodeForAnyNode\n";
        return;
    }
    if (node->nodekind == StmtK)
        generateCodeForStmtNode(node);
    else
        generateCodeForExprNode(node);
}

void CodeGenerator::generateCode(TreeNode *node)
{
    if (shouldShowVisitingMessages)
        std::cout << "generateFunction\n";
    if (node == NULL)
    {
        if (shouldShowVisitingMessages)
            std::cout << "This node is NULL, exiting generateCode\n";
        return;
    }
    if (shouldShowVisitingMessages)
        printNode(node);
    generateCodeForAnyNode(node);
    if (shouldShowVisitingMessages)
        std::cout << "generateCode visiting brother\n";
    generateCode(node->sibling);
}

BucketList getRecordFromSymbleTableAtScope(TreeNode *node)
{
    return st_find_at_scope(
        node->attr.name,
        node->scope);
}

BucketList getRecordFromSymbleTableAtGlobalScope(TreeNode *node)
{
    return st_find_at_scope(node->attr.name, "global");
}

BucketList getRecordFromSymbleTable(TreeNode *node)
{
    return st_find(node->attr.name,
                   node->scope);
}
void CodeGenerator::
    generateCodeForBranch(std::string branch_name, ConditionCodes condition)
{
    if (shouldShowVisitingMessages)
        std::cout << "+++++++++++++ Branch start +++++++++++++"
                  << "\n";
    print(
        moveLowToHigh(
            AcumulatorRegister,
            SwapRegister));

    Instruction *leftByte = loadImediateToRegister(TemporaryRegister, 0);

    Instruction *rightByte = loadImediateToRegister(AcumulatorRegister, 0);

    print(leftByte);

    print(
        new TypeAInstruction(
            1,
            "LSL",
            8,
            TemporaryRegister,
            TemporaryRegister));

    print(rightByte);

    print(
        new TypeBInstruction(
            4,
            "ADD",
            TemporaryRegister,
            AcumulatorRegister,
            TemporaryRegister));

    print(new TypeBInstruction(
        4,
        "ADD",
        TemporaryRegister,
        BaseAddressRegister,
        TemporaryRegister));

    BranchLabel *branchLabel = new BranchLabel(branch_name, leftByte, rightByte);

    print(
        moveHighToLow(AcumulatorRegister, SwapRegister));

    print(new TypeFInstruction(38, "BX", condition, TemporaryRegister));

    labelOriginMap[branch_name] = branchLabel;
    if (shouldShowVisitingMessages)
        std::cout << "+++++++++++++ Branch end +++++++++++++"
                  << "\n";
}

void CodeGenerator::generateCodeForStmtNode(TreeNode *node)
{
    if (node == NULL)
    {
        if (shouldShowVisitingMessages)
            std::cout << "Thid node is NULL, exiting\n";
        return;
    }
    if (shouldShowVisitingMessages)
        printNode(node); //Check visited node
    if (shouldShowVisitingMessages)
        std::cout << "This is a statement\n";
    switch (node->kind.stmt)
    {

    case CompoundK:
        generateCode(node->child[0]);
        generateCode(node->child[1]);
        break;

    case VetDeclK:

        fetchVarOffset(node, AcumulatorRegister);

        print(
            new TypeBInstruction(
                40,
                "STR",
                node->scope == "global" ? GlobalPointer : FramePointer,
                AcumulatorRegister,
                HeapArrayRegister));

        print(
            loadImediateToRegister(
                TemporaryRegister,
                node->attr.val));
        print(
            new TypeAInstruction(
                1,
                "LSL",
                2,
                TemporaryRegister,
                TemporaryRegister));
        print(
            new TypeBInstruction(
                4,
                "ADD",
                TemporaryRegister,
                HeapArrayRegister,
                HeapArrayRegister

                ));

        break;

    case VarDeclK:
        if (shouldShowVisitingMessages)
            std::cout << "Variable " << node->attr.name << "\n";

        break;

    case WhileK:
    {
        //Label names
        std::string while_label = "while_" + std::to_string(node->attr.val);
        std::string do_label = "w_do_" + std::to_string(node->attr.val);
        std::string while_end_label = "w_end_" + std::to_string(node->attr.val);

        printLabelNop(while_label);

        generateCode(node->child[0]);
        generateCodeForBranch(
            do_label,
            translateCondition(node->child[0]->attr.op));

        generateCodeForBranch(
            while_end_label,
            AL);

        printLabelNop(do_label);

        generateCode(node->child[1]);
        generateCodeForBranch(while_label, AL);

        printLabelNop(while_end_label);
    }
    break;

    case IfK:
    {
        generateCode(node->child[0]);
        std::string if_true_label_name = "if_true_" + std::to_string(node->attr.val);
        std::string if_end_label_name = "if_end_" + std::to_string(node->attr.val);
        generateCodeForBranch(if_true_label_name,
                              translateCondition(
                                  node->child[0]->attr.op));

        // Generate code for ELSE code

        if (node->child[2] != NULL)
            generateCode(node->child[2]);

        generateCodeForBranch(if_end_label_name, AL);

        printLabelNop(if_true_label_name);

        // Generate code for THEN code

        generateCode(node->child[1]);

        printLabelNop(if_end_label_name);
    }
    break;

    case ReturnK:
    {

        if (node->child[0] != NULL)
        {
            generateCode(node->child[0]);
        }
        else
        {
            print(
                loadImediateToRegister(AcumulatorRegister, 0));
        }
        generateCodeForBranch("end_" + node->scope, AL);
    }
    break;

    case FunDeclK:
    {
        std::string FunctionName = std::string(node->attr.name);
        if (
            (FunctionName != "input") &&
            (FunctionName != "output") &&
            (FunctionName != "outputLED"))
        {
            if (shouldShowVisitingMessages)
                hr(node->attr.name);

            Instruction *labelInstruction = pushRegister(ReturnAddressRegister);
            registerLabelInstruction(FunctionName, labelInstruction);

            print(labelInstruction);
            print(
                new TypeDInstruction(
                    57,
                    "ADD",
                    FramePointer,
                    0));

            if (node->child[1] != NULL)
            {
                generateCode(node->child[1]);
            }
            print(
                popRegister(ReturnAddressRegister));

            DestroyARAndExitFunction(node);
        }
    }
    break;

    case FunActiveK:
    {
        std::string FunctionName = std::string(node->attr.name);
        std::string func_decl_label = "function_" + FunctionName;
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
                generateCodeForFunctionActivation(node);
            }
        }
    }
    break;

    case AssignK:
    {

        std::cout << "This is Assign\n";

        TreeNode *varToBeAssignedInto = node->child[0];
        std::cout << "Back to assign from value\n";
        switch (varToBeAssignedInto->kind.exp)
        {
        case IdK:
        {
            std::cout << "VARIABLE\n";
            generateCode(node->child[1]);
            fetchVarOffset(varToBeAssignedInto, TemporaryRegister);
            print(
                new TypeBInstruction(
                    40,
                    "STR",
                    varToBeAssignedInto->scope == "global"
                        ? GlobalPointer
                        : FramePointer,
                    TemporaryRegister,
                    AcumulatorRegister));
        }
        break;
        case VetK:
        {
            std::cout << "VECTOR\n";
            loadVariable(varToBeAssignedInto, TemporaryRegister);
            print(
                pushRegister(TemporaryRegister));
            generateCode(varToBeAssignedInto->child[0]);
            print(
                pushAcumulator());
            generateCode(node->child[1]);
            print(
                moveLowToHigh(AcumulatorRegister, SwapRegister));
            print(popRegister(AcumulatorRegister));
            print(popRegister(TemporaryRegister));
            print(
                new TypeBInstruction(
                    4,
                    "ADD",
                    AcumulatorRegister,
                    TemporaryRegister,
                    TemporaryRegister));
            print(
                moveHighToLow(
                    AcumulatorRegister,
                    SwapRegister));
            print(
                new TypeAInstruction(
                    48,
                    "STR",
                    0,
                    TemporaryRegister,
                    AcumulatorRegister));

            print(
                moveLowToHigh(AcumulatorRegister, SwapRegister));
        }
        break;

        default:
            std::cout << "&$#$*&$#@#$*( ERROR assigning into "
                      << node->child[0]->kind.exp
                      << "@#$*(*&$#$*\n";
            break;
        }

        // if (shouldShowVisitingMessages)
        std::cout << "Assign end\n";
    }
    break;

    default:
        // if (shouldShowVisitingMessages)
        std::cout
            << "Code not generated for this node\n";
        break;
    }
}

void CodeGenerator::generateCodeForExprNode(TreeNode *node)
{
    if (shouldShowVisitingMessages)
        std::cout << "This is an expression\n";
    if (shouldShowVisitingMessages)
        printNode(node);
    switch (node->kind.stmt)
    {
    case OpK:
    {

        generateCode(node->child[0]);

        print(
            pushAcumulator());

        generateCode(node->child[1]);

        print(
            moveLowToLowRegister(
                AcumulatorRegister,
                TemporaryRegister));

        print(
            popRegister(
                AcumulatorRegister));

        generateOperationCode(node);
    }
    break;
    case ConstK:
    {

        std::string fullNumber, byteNumber;
        fullNumber = std::bitset<32>(node->attr.val).to_string();

        for (int i = 0; i < 4; i++)
        {
            byteNumber = fullNumber.substr(8 * i, 8);
            std::bitset<8> partialNumber(byteNumber);
            int byteAsInt = (int)partialNumber.to_ulong();

            if (i == 0)
            {
                print(loadImediateToRegister(AcumulatorRegister, byteAsInt));
            }
            else
            {
                print(
                    new TypeAInstruction(
                        1,
                        "LSL",
                        8,
                        AcumulatorRegister,
                        AcumulatorRegister));
                if (byteAsInt != 0)
                {
                    print(
                        loadImediateToRegister(TemporaryRegister, byteAsInt));
                    print(
                        new TypeBInstruction(
                            4,
                            "ADD",
                            TemporaryRegister,
                            AcumulatorRegister,
                            AcumulatorRegister));
                }
            }
        }
    }
    break;
    case IdK:
    {
        loadVariable(node, AcumulatorRegister);
    }
    break;

    case VetK:
    {
        generateCode(node->child[0]);
        loadVariable(node, TemporaryRegister);
        print(
            new TypeBInstruction(
                44,
                "LDR",
                TemporaryRegister,
                AcumulatorRegister,
                AcumulatorRegister));
    }
    break;

    default:
        // if (shouldShowVisitingMessages)
        std::cout << "Code not generated for this expr\n";
        break;
    }
}
void CodeGenerator::generateOperationCode(TreeNode *node)
{
    if (node == NULL)
    {
        if (shouldShowVisitingMessages)
            std::cout << "Thid node is NULL, exiting\n";
        return;
    }
    if (shouldShowVisitingMessages)
        printNode(node); //Check visited node
    switch (node->attr.op)
    {
    case PLUS:
        print(
            new TypeBInstruction(
                4,
                "ADD",
                TemporaryRegister,
                AcumulatorRegister,
                AcumulatorRegister));
        break;

    case MINUS:
        print(
            new TypeBInstruction(
                5,
                "SUB",
                TemporaryRegister,
                AcumulatorRegister,
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
                AcumulatorRegister,
                TemporaryRegister));

        break;
    }
}

//Headers and Footers

void CodeGenerator::createHeader()
{
    int headerSize = 1;
    print(
        new TypeDInstruction(56, "ADD", BaseAddressRegister, headerSize - 1));
    generateRunTimeSystem();

    if (shouldShowVisitingMessages)
        hr("Code above me is header code");
}

void CodeGenerator::createFooter()
{
    if (shouldShowVisitingMessages)
        std::cout << "This is a footer\n";
}

//Instruction Class
std::string Instruction::to_string()
{
    return "Generic Instruction";
}

Instruction *jumpToRegister(Registers reg)
{
    return new TypeFInstruction(
        38,
        "BX",
        AL,
        reg

    );
}

Instruction *popRegister(Registers reg)
{
    return new TypeEInstruction(
        68,
        "POP",
        0,
        reg);
}

Instruction *pushRegister(Registers reg)
{
    return new TypeEInstruction(
        67,
        "PUSH",
        0,
        reg);
}

Instruction *loadImediateToRegister(Registers regis, int number)
{
    return new TypeDInstruction(
        8,
        "MOV",
        regis,
        number);
}

Instruction *pushAcumulator()
{
    return pushRegister(AcumulatorRegister);
}

Instruction *nop()
{
    return new TypeDInstruction(74, "NOP", 0, 0);
}

Instruction *moveLowToLowRegister(Registers origin, Registers destination)
{
    return new TypeCInstruction(
        6,
        "ADD",
        0,
        origin,
        destination);
}

Instruction *moveLowToHigh(Registers low, Registers high)
{
    return new TypeEInstruction(
        36,
        "MOV",
        low,
        high);
}

Instruction *moveHighToLow(Registers low, Registers high)
{
    return new TypeEInstruction(
        35,
        "MOV",
        high,
        low);
}

void CodeGenerator::loadVariable(TreeNode *node, Registers reg)
{
    fetchVarOffset(node, reg);
    print(
        new TypeBInstruction(
            44,
            "LDR",
            reg,
            node->scope == "global"
                ? GlobalPointer
                : FramePointer,
            reg));
}
void CodeGenerator::fetchVarOffset(TreeNode *node, Registers reg)
{
    BucketList record = getRecordFromSymbleTable(node);
    print(
        loadImediateToRegister(reg, record->memloc * 4));
    if (node->scope != "global")
    {
        print(
            new TypeEInstruction(
                21,
                "NEG",
                reg,
                reg));
    }
}

void hr(std::string middle)
{
    std::string rule = "-----------------------------";
    std::cout << rule << " FUNCTION " + middle + " " << rule << "\n";
}

void CodeGenerator::DestroyARAndExitFunction(TreeNode *node)
{
    DataSection ds;
    int variableCountInFunction = ds.getSize(node->attr.name);
    std::string label = "end_" + node->attr.name;
    printLabelNop(label);
    for (int recordInAR = 0; recordInAR < variableCountInFunction; recordInAR++)
    {
        print(
            popRegister(TemporaryRegister));
    }
    print(
        popRegister(FramePointer));
    print(
        jumpToRegister(ReturnAddressRegister));
}

void CodeGenerator::registerLabelInstruction(std::string label, Instruction *Instruction)
{
    Instruction->setlabel(label);
    labelDestMap[label] = Instruction;
}

void CodeGenerator::printLabelNop(std::string label)
{
    Instruction *nopInstruction = nop();
    registerLabelInstruction(label, nopInstruction);
    print(nopInstruction);
}
void CodeGenerator::generateGlobalAR()
{
    DataSection ds;
    int globalCount = ds.getSize("global");
    print(
        loadImediateToRegister(AcumulatorRegister, 0));

    for (
        int i = 0;
        i < globalCount + 1;
        i++)
    {
        print(pushAcumulator());
        if (i == 0)
            print(
                new TypeDInstruction(
                    57,
                    "ADD",
                    GlobalPointer,
                    0));
    }
}
void CodeGenerator::generateCodeForFunctionActivation(TreeNode *node)
{
    DataSection ds;
    std::string FunctionName = node->attr.name;
    int variableCountInFunction = ds.getSize(FunctionName);
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
        argumentNode = argumentNode->sibling;
    }
    //All variables pushed
    // Following code replaces the JUMP AND LINK INSTRUCTION (jal label)
    int numberOfCodeLinesBetweenARBuildAndFunctionExecution = 8; //Might be changed
    print(
        new TypeDInstruction(
            56,
            "ADD",
            ReturnAddressRegister,
            numberOfCodeLinesBetweenARBuildAndFunctionExecution - 1));
    generateCodeForBranch(FunctionName, AL);
}

void CodeGenerator::generateRunTimeSystem()
{
    generateGlobalAR();
    generateCodeForFunctionActivation(mainActivation);
    destroyGlobalAR();
    print(
        //What the program will do after main
        new TypeDInstruction(
            75,
            "HLT",
            0,
            0));
}

void CodeGenerator::destroyGlobalAR()
{
    DataSection ds;
    int globalCount = ds.getSize("global");
    for (
        int i = 0;
        i != globalCount;
        i++)
    {
        print(popRegister(TemporaryRegister));
    }
}

void CodeGenerator::generateBinaryCode()
{
    printf("\n\n +++++ Code generator! +++++ \n\n");

    std::ofstream outputStream("output.txt");
    int ramCounter = 0;
    for (Instruction *inst : code)
    {
        std::string bin = inst->to_binary();
        assert(bin.size() == 16);
        printf("% 3d: % -22s => %s\n", inst->relativeAddress, inst->to_string().c_str(), bin.c_str());
        outputStream << "RAM[" << ramCounter++ << "] <= 8'b" << bin.substr(0, 8) << std::endl;
        outputStream << "RAM[" << ramCounter++ << "] <= 8'b" << bin.substr(8, 16) << std::endl;
    }
}

std::string printRegister(int reg)
{
    switch (reg)
    {
    case HeapArrayRegister:
        return "$H0";
    case AcumulatorRegister:
        return "$A0";
    case TemporaryRegister:
        return "$T1";
    case FramePointer:
        return "$FP";
    case GlobalPointer:
        return "$GP";
    case BaseAddressRegister:
        return "$BA";
    case ReturnAddressRegister:
        return "$RA";
    case SwapRegister:
        return "$SR";
    }
    return "!UNKNOWN!";
}