#include "codegenerator.h"
#include "globals.h"
#include "util.h"
#include "symtab.h"
#include "memory.h"
#include <iostream>
#include <assert.h>
#include <fstream>
#include "mifgenerator.h"
#include "bytes.h"
#include "instructions.h"

ConditionCodes translateCondition(TokenType operation)
{

    switch (operation)
    {
    case LESSER:
        return CC;
    case LESSEQ:
        return LS;
    case GREATER:
        return HI;
    case GREATEQ:
        return CS;
    case EQCOMP:
        return EQ;
    case NOTEQ:
        return NE;
    default:
        return AL;
    }
}

//Code Generator Class
CodeGenerator::CodeGenerator(bool displayable, int programOffset)
{
    shouldPrintGeneratedCodeOnScreen = displayable;
    shouldShowVisitingMessages = false;
    mainActivation = newStmtNode(FunActiveK);
    mainActivation->attr.name = "fun_main";
    mainActivation->type = Integer;
    mainActivation->attr.val = 0;
    mainActivation->child[0] = NULL;
    mainActivation->scope = "global";
    this->programOffset = programOffset;
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

void CodeGenerator::setDebugName(std::string name)
{
    if (code.size() > 0)
    {
        Instruction *lastInst = code.back();
        lastInst->debugname = name;
    }
}

void CodeGenerator::generate(TreeNode *node)
{
    createHeader();
    generateCode(node);
    createFooter();
}

void CodeGenerator::linker()
{
    printEveryLabelLink();

    insertIndexInsideEveryInstruction();

    for (auto origin : labelOriginMap)
    {
        std::string label = origin.first;
        for (BranchLabel *label_dest : origin.second)
        {
            int destinationAddress = labelDestMap[label]->relativeAddress;
            Bytes number = Bytes(destinationAddress);
            label_dest->firstByte->immediate = number.getNthByte(2);
            label_dest->secondByte->immediate = number.getNthByte(3);

            // Prints
            std::cout << "LINKER PRINTS\n";
            std::cout << label << " -> " << label_dest->to_string() << "\n";
            std::cout << "destinationAddress: " << destinationAddress << "\n";
            std::cout << "first Byte : " << label_dest->firstByte->to_string() << "\n";
            std::cout << "second Byte: " << label_dest->secondByte->to_string() << "\n";
        }
    }
}

void CodeGenerator::insertIndexInsideEveryInstruction()
{
    int i = 0;
    for (Instruction *inst : code)
    {
        inst->relativeAddress = i++;
    }
}

void CodeGenerator::printEveryLabelLink()
{
    for (auto item : labelDestMap)
    {
        std::string label = item.first;
        Instruction *label_dest = item.second;
        std::cout << label << " -> " << label_dest->name << "\n";
    }
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

BucketList getRecordFromSymbolTableAtScope(TreeNode *node)
{
    return st_find_at_scope(
        node->attr.name,
        node->scope);
}

BucketList getRecordFromSymbolTableAtGlobalScope(TreeNode *node)
{
    return st_find_at_scope(node->attr.name, "global");
}

BucketList getRecordFromSymbolTable(TreeNode *node)
{
    return st_find(node->attr.name,
                   node->scope);
}

void CodeGenerator::generateCodeForBranch(std::string branch_name,
                                          ConditionCodes condition,
                                          TreeNode *operationNode)
{

    if (shouldShowVisitingMessages)
        std::cout << "+++++++++++++ Branch start +++++++++++++\n";

    print(moveLowToHigh(AcumulatorRegister, SwapRegister));

    setDebugName("begin Branch");

    BranchLabel * branchLabel = new BranchLabel(branch_name, condition);

    print(branchLabel->firstByte);

    print(
        new TypeAInstruction(
            1,
            "LSL",
            8,
            TemporaryRegister,
            TemporaryRegister));

    print(branchLabel->secondByte);

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

    print(
        new TypeEInstruction(
            59,
            "SXTH",
            TemporaryRegister,
            TemporaryRegister));

    print(moveHighToLow(AcumulatorRegister, SwapRegister));

    if (operationNode)
    {
        print(pushRegister(TemporaryRegister));
        generateCode(operationNode); // generate child 0 and child 1 and then CMP
        generateCodeForPop(TemporaryRegister);
    }

    print(branchLabel->branch);

    setDebugName("end Branch");

    labelOriginMap[branch_name].push_back(branchLabel);

    if (shouldShowVisitingMessages)
        std::cout << "+++++++++++++ Branch end +++++++++++++\n";
}

void CodeGenerator::generateCodeForPop(Registers reg)
{
    print(popRegister(reg));
    print(nop());
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
        setDebugName("begin VetDeclK " + node->attr.val);

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
        setDebugName("end VetDeclK " + node->attr.val);
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

        print(
            new TypeDInstruction(
                56,
                "ADD",
                ReturnAddressRegister,
                0));
        setDebugName(while_label);
        print(pushRegister(ReturnAddressRegister));

        generateCodeForBranch( // If cond, then go to do_label
            do_label,
            translateCondition(node->child[0]->attr.op),
            node->child[0]);

        generateCodeForBranch( // Else, go to while_end_label
            while_end_label,
            AL);

        printLabelNop(do_label); // Goes here if cond is true

        generateCode(node->child[1]);

        print(popRegister(ReturnAddressRegister));
        setDebugName("Return to " + while_label);
        print(nop());
        print(
            new TypeFInstruction(
                38,
                "BX",
                AL,
                ReturnAddressRegister));
        // generateCodeForBranch(while_label, AL); // Go back to first branch

        printLabelNop(while_end_label);
        print(popRegister(ReturnAddressRegister));
        print(nop());
    }
    break;

    case IfK:
    {
        std::string if_true_label_name = "if_true_" + std::to_string(node->attr.val);
        std::string if_end_label_name = "if_end_" + std::to_string(node->attr.val);

        generateCodeForBranch(if_true_label_name,
                              translateCondition(node->child[0]->attr.op),
                              node->child[0]);

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
            generateCode(node->child[0]);
        else
            print(
                loadImediateToRegister(AcumulatorRegister, 0));

        generateCodeForBranch("end_" + node->scope, AL);
    }
    break;

    case FunDeclK:
    {
        std::string FunctionName = std::string(node->attr.name);
        if (
            (FunctionName != "fun_input") &&
            (FunctionName != "fun_output"))
        {
            if (shouldShowVisitingMessages)
                hr(node->attr.name);

            Instruction *labelInstruction = pushRegister(ReturnAddressRegister);
            registerLabelInstruction(FunctionName, labelInstruction);

            print(labelInstruction);
            setDebugName("begin FunDeclK " + FunctionName);
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

            DestroyARAndExitFunction(node);
            setDebugName("end FunDeclK " + FunctionName);
        }
    }
    break;

    case FunActiveK:
    {
        std::string FunctionName = std::string(node->attr.name);
        generateCode(node->child[0]);
        if (FunctionName == "fun_input")
        {
            print(
                new TypeEInstruction(
                    70,
                    "PAUSE",
                    0,
                    0));
            print(
                new TypeEInstruction(
                    71,
                    "INPUT",
                    0,
                    AcumulatorRegister));
            setDebugName("INPUT");
        }
        else if (FunctionName == "fun_output")
        {
            print(
                new TypeEInstruction(
                    70,
                    "PAUSE",
                    0,
                    0));
            print(
                outputRegister(AcumulatorRegister));
            setDebugName("OUTPUT");
        }
        else
            generateCodeForFunctionActivation(node);
    }
    break;

    case AssignK:
    {
        if (shouldShowVisitingMessages)
            std::cout << "This is Assign\n";

        TreeNode *varToBeAssignedInto = node->child[0];
        if (shouldShowVisitingMessages)
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
            generateCodeForPop(AcumulatorRegister);
            generateCodeForPop(TemporaryRegister);
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

        if (shouldShowVisitingMessages)
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

        generateCodeForPop(AcumulatorRegister);

        generateOperationCode(node);
    }
    break;
    case ConstK:
    {
        generateCodeForConst(node->attr.val);
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
        print(nop());
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
                TemporaryRegister,
                AcumulatorRegister));

        break;
    }
}

//Headers and Footers

void CodeGenerator::createHeader()
{
    int headerSize = 0;
    print(nop());
    print(
        new TypeDInstruction(56, "ADD", BaseAddressRegister, headerSize));
    print(subImeditateFromRegister(1, BaseAddressRegister));
    // print(
    //         new TypeEInstruction(
    //             69,
    //             "OUTSS",
    //             0,
    //             BaseAddressRegister));
    setDebugName("begin Header");
    generateRunTimeSystem();

    if (shouldShowVisitingMessages)
        hr("Code above me is header code");
    setDebugName("end Header");
}

void CodeGenerator::createFooter()
{
    if (shouldShowVisitingMessages)
        std::cout << "This is a footer\n";
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
    print(nop());
}

void CodeGenerator::fetchVarOffset(TreeNode *node, Registers reg)
{
    BucketList record = getRecordFromSymbolTable(node);
    print(
        loadImediateToRegister(reg, record->memloc));
    // if (node->scope != "global")
    // {
    //     print(
    //         new TypeEInstruction(
    //             21,
    //             "NEG",
    //             reg,
    //             reg));
    // }
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

    //Return redirects to here
    std::string label = "end_" + node->attr.name;
    printLabelNop(label);

    generateCodeForPop(ReturnAddressRegister);
    for (int recordInAR = 0; recordInAR < variableCountInFunction; recordInAR++)
    {
        generateCodeForPop(TemporaryRegister);
    }

    generateCodeForPop(FramePointer);

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
    setDebugName("begin GlobalAR");
    for (
        int i = 0;
        i < globalCount + 1;
        i++)
    {
        print(pushAcumulator());
    }

    print(
        new TypeDInstruction(
            57,
            "ADD",
            GlobalPointer,
            0));

    setDebugName("end GlobalAR");
}

void CodeGenerator::generateCodeForFunctionActivation(TreeNode *node)
{
    DataSection ds;
    std::string FunctionName = node->attr.name;
    int variableCountInFunction = ds.getSize(FunctionName);
    int argumentCount = node->attr.val;
    print(
        pushRegister(FramePointer));
    setDebugName("begin Activation " + FunctionName);

    int localVariableCount = variableCountInFunction - argumentCount;

    //Build activation record

    //Declaring local variables, if any
    if (localVariableCount > 0)
    {
        print(
            new TypeDInstruction(
                8,
                "MOV",
                AcumulatorRegister,
                0));

        // Inserting the local vars into the AR
        for (int i = 0; i < localVariableCount; ++i)
        {
            print(pushAcumulator());
        }
    }

    // Inserting the arguments into the AR
    TreeNode *argumentNode = node->child[0];
    for (int i = 0; i < argumentCount; i++)
    {
        generateCodeForAnyNode(argumentNode);
        print(pushAcumulator());
        argumentNode = argumentNode->sibling;
    }

    //All variables pushed
    // Following code replaces the JUMP AND LINK INSTRUCTION (jal label)
    int branchCodeStart = code.size();
    Instruction *jumpAndLinkAddress =
        new TypeDInstruction(56,
                             "ADD",
                             ReturnAddressRegister,
                             0);
    print(jumpAndLinkAddress);

    generateCodeForBranch(FunctionName, AL);

    jumpAndLinkAddress->immediate = (code.size() - branchCodeStart);

    setDebugName("end Activation " + FunctionName);
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
        i < globalCount + 1;
        i++)
    {
        generateCodeForPop(TemporaryRegister);
    }
}

void CodeGenerator::generateBinaryCode(std::string outputFile)
{
    printf("\n\n +++++ Code generator! +++++ \n\n");

    mif.open(outputFile);

    if (programOffset)
        generateCodeToJumpToOS();

    for (Instruction *inst : code)
    {
        std::string bin = inst->to_binary();
        assert(bin.size() == 16);

        if (!inst->debugname.empty())
            printf("%s\n", inst->debugname.c_str());

        printf("% 3d: %-22s => %s\n",
               inst->relativeAddress,
               inst->to_string().c_str(),
               bin.c_str());

        mif.printInstruction(inst->relativeAddress + programOffset,
                             bin,
                             inst->to_string());

        if (!inst->debugname.empty())
            mif.printDebugMsg(inst->debugname);

        mif.jumpLine();
    }

    mif.printMultipleEmptyPosition(code.size() + programOffset, 16384);

    mif.printFooter();
    printf("\n\n Output saved on %s \n\n", outputFile.c_str());
}

void CodeGenerator::generateCodeToJumpToOS()
{
    std::vector<Instruction *> originalCode = code, newCode;
    std::string originalInst = generatedCode, ngc;
    code = newCode;
    generatedCode = ngc;
    generateCodeForConst(2048);

    print(new TypeFInstruction(38, "BX", AB, AcumulatorRegister));

    Instruction *inst;

    for (int i = 0; i < (int)code.size(); i++)
    {
        inst = code[i];
        std::string bin = inst->to_binary();
        assert(bin.size() == 16);

        this->mif.printInstruction(i,
                                   bin,
                                   inst->to_string());

        this->mif.jumpLine();
    }

    mif.printMultipleEmptyPosition(code.size(), programOffset);
    code = originalCode;
    generatedCode = originalInst;
}

void CodeGenerator::generateCodeForConst(int value)
{
    {

        Bytes number = Bytes(value);

        for (int i = 0; i < 4; i++)
        {
            int byte = number.getNthByte(i);

            if (i == 0)
            {
                print(loadImediateToRegister(AcumulatorRegister, byte));
                setDebugName("begin ConstK");
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
                if (byte != 0)
                {
                    print(
                        loadImediateToRegister(TemporaryRegister, byte));
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
        setDebugName("end ConstK");
    }
}
