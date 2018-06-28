#include "codegenerator.h"
#include "globals.h"
#include "util.h"
#include "symtab.h"
#include "memory.h"
#include <iostream>
#include <bitset>

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
}

void CodeGenerator::print(Instruction *instruction)
{
    std::string inst = instruction->to_string_with_label();
    if (shouldPrintGeneratedCodeOnScreen)
    {
        std::cout << inst;
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
    //     std::cout << inst->to_string();
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
void CodeGenerator::generateCodeForBranch(std::string branch_name, ConditionCodes condition)
{
    std::cout << "+++++++++++++ Branch start +++++++++++++"
              << "\n";
    print(
        new TypeEInstruction(
            36,
            "MOV",
            AcumulatorRegister,
            SwapRegister));

    TypeDInstruction *leftByte = new TypeDInstruction(8, "MOV", TemporaryRegister, 0);

    TypeDInstruction *rightByte = new TypeDInstruction(8, "MOV", TemporaryRegister, 0);

    print(leftByte);

    print(
        new TypeAInstruction(
            1,
            "LSL",
            8,
            TemporaryRegister,
            TemporaryRegister));

    print(rightByte);

    print(new TypeBInstruction(
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
        new TypeEInstruction(
            35,
            "MOV",
            SwapRegister,
            AcumulatorRegister));

    print(new TypeFInstruction(38, "BX", condition, TemporaryRegister));

    labelOriginMap[branch_name] = branchLabel;
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
        // std::cout << "Vector size: " << node->attr.val << "\n";
        // std::cout << "Location: " << getRecordFromSymbleTableAtScope(node)->memloc << "\n";
        //TODO GENERATE CODE
        print(new TypeCInstruction(
            6,
            "ADD",
            0,
            HeapArrayRegister,
            AcumulatorRegister));
        print(loadImediateToRegister(TemporaryRegister, getRecordFromSymbleTableAtScope(node)->memloc * 4));
        print(new TypeBInstruction(
            40,
            "STR",
            TemporaryRegister,
            AcumulatorRegister,
            AcumulatorRegister));
        print(
            loadImediateToRegister(
                TemporaryRegister,
                node->attr.val * 4));
        print(new TypeBInstruction(
            4,
            "ADD",
            TemporaryRegister,
            AcumulatorRegister,
            HeapArrayRegister

            ));

        break;

    case VarDeclK:
        if (shouldShowVisitingMessages)
            std::cout << "Variable " << node->attr.name << "\n";

        break;
    case WhileK:
        generateCode(node->child[1]);
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

        Instruction *if_true_dest = nopWithLabel(if_true_label_name);
        print(if_true_dest);

        // Generate code for THEN code

        generateCode(node->child[1]);

        Instruction *if_end_dest = nopWithLabel(if_end_label_name);
        print(if_end_dest);

        // Adding labels to map
        labelDestMap[if_true_label_name] = if_true_dest;
        labelDestMap[if_end_label_name] = if_end_dest;
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

            hr(node->attr.name);
            Instruction *labelInstruction = pushRegister(ReturnAddressRegister);
            labelInstruction->setlabel("function_" + std::string(node->attr.name));

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
                    argumentNode = argumentNode->sibling;
                }
                //All variables pushed
                // Following code replaces the JUMP AND LINK INSTRUCTION (jal label)
                int numberOfCodeLinesBetweenARBuildAndFunctionExecution = 3; //Might be changed
                print(
                    new TypeDInstruction(
                        56,
                        "ADD",
                        ReturnAddressRegister,
                        numberOfCodeLinesBetweenARBuildAndFunctionExecution - 1));
                //  Branching to function definition
                //  If number of prints below changes, ...
                //  ... change the value of variable ...
                //  ... numberOfCodeLinesBetweenARBuildAndFunctionExecution
                print(
                    loadImediateToRegister(
                        TemporaryRegister,
                        (getRecordFromSymbleTableAtGlobalScope(node)->memloc) * 4));
                print(
                    new TypeBInstruction(
                        44,
                        "LDR",
                        TemporaryRegister,
                        GlobalPointer,
                        TemporaryRegister));
                print(
                    jumpToRegister(TemporaryRegister));
            }
        }
    }
    break;

    case AssignK:
    {
        // if (shouldShowVisitingMessages)
        std::cout << "This is Assign\n";
        // printNode(node->child[0]);
        printNode(node->child[1]);
        BucketList record = getRecordFromSymbleTable(node->child[0]);

        generateCode(node->child[1]);

        TreeNode *varToBeAssignedInto = node->child[0];
        std::cout << "Back to assign from value\n";
        switch (varToBeAssignedInto->kind.exp)
        {
        case IdK:
        {
            std::cout << "VARIABLE\n";
            print(
                loadImediateToRegister(TemporaryRegister, record->memloc * 4));
            if (varToBeAssignedInto->scope != "global")
                print(
                    new TypeEInstruction(
                        21,
                        "NEG",
                        TemporaryRegister,
                        TemporaryRegister));
            print(
                new TypeBInstruction(
                    40,
                    "STR",
                    varToBeAssignedInto->scope == "global" ? GlobalPointer : FramePointer,
                    TemporaryRegister,
                    AcumulatorRegister
                )
            );
        }
        break;
        case VetK:
            std::cout << "VECTOR\n";
            break;

        default:
            std::cout << "&$#$*&$#@#$*( ERROR assigning into "
                      << node->child[0]->kind.exp
                      << "@#$*(*&$#$*\n";
            break;
        }

        std::cout << record->memloc << "\n";

        // if (shouldShowVisitingMessages)

        generateCode(node->child[1]);
    }
    break;

    default:
        if (shouldShowVisitingMessages)
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
            moveToLowRegister(
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

    default:
        if (shouldShowVisitingMessages)
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
    print(
        new TypeDInstruction(56, "ADD", BaseAddressRegister, 0));
    if (shouldShowVisitingMessages)
        std::cout << "This is a header\n";
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

Instruction *pushRegister(int reg)
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

Instruction *nopWithLabel(std::string label)
{
    Instruction *temp = new TypeDInstruction(74, "NOP", 0, 0);
    temp->setlabel(label);

    return temp;
}

Instruction *moveToLowRegister(Registers origin, Registers destination)
{
    return new TypeCInstruction(
        6,
        "ADD",
        0,
        origin,
        destination);
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