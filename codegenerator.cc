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

void CodeGenerator::setMode(bool bios, bool compressed, bool os)
{
    isBios = bios;
    isCompressedProgram = compressed;
    isOS = os;
    memorySize = isBios ? 512 : 16384;
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
            int destinationAddress =
                labelDestMap[label]->relativeAddress -
                label_dest->branch->relativeAddress;
            Bytes number = Bytes(destinationAddress);
            label_dest->firstByte->immediate = number.getNthByte(2);
            label_dest->secondByte->immediate = number.getNthByte(3);

            // Prints
            std::cout << "LINKER PRINTS\n";
            std::cout << label << " -> " << label_dest->to_string() << "\n";
            std::cout << "destinationAddress: " << destinationAddress << "\n";
            std::cout << "first Byte : " << label_dest->firstByte->to_string() << "\n";
            std::cout << "second Byte: " << label_dest->secondByte->to_string() << "\n";
            std::cout << "branch position: " << label_dest->branch->relativeAddress + programOffset << "\n";
            std::cout << "absolute position: " << labelDestMap[label]->relativeAddress + programOffset << "\n";
        }
    }
}

void CodeGenerator::insertIndexInsideEveryInstruction()
{
    int i = 0;
    for (Instruction *inst : code)
        inst->relativeAddress = i++;
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

    BranchLabel *branchLabel = new BranchLabel(branch_name, condition);

    print(branchLabel->firstByte);
    setDebugName("begin Branch");
    print(leftShiftImmediate(TemporaryRegister, 8));
    print(branchLabel->secondByte);
    print(signExtendHW(TemporaryRegister));

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
void CodeGenerator::generateCodeForIf(TreeNode *node)
{
    std::string if_end = "if_end_" +
                         std::to_string(node->attr.val);
    TreeNode *condition = node->child[0];
    TreeNode *body = node->child[1];
    Instruction *branch =
        branchImmediate(translateCondition(condition->attr.op), 9);
    generateCode(condition);
    print(branch);
    int size = code.size();
    generateCodeForBranch(if_end, AL);
    branch->offset = code.size() - size + 1;
    generateCode(body);
    printLabelNop(if_end);
    setDebugName("end Elseless IF");
}

void CodeGenerator::generateCodeForIfElse(TreeNode *node)
{
    std::string if_true_label_ = "if_true_" + std::to_string(node->attr.val);
    std::string if_end_label = "if_end_" + std::to_string(node->attr.val);
    TreeNode *condition = node->child[0];
    TreeNode *body = node->child[1], *elseBody = node->child[2];
    generateCodeForBranch(if_true_label_,
                          translateCondition(condition->attr.op),
                          condition);

    generateCode(elseBody);

    generateCodeForBranch(if_end_label, AL);

    printLabelNop(if_true_label_);

    generateCode(body);

    printLabelNop(if_end_label);
}

void CodeGenerator::generateCodeForPop(Registers reg)
{
    print(popRegister(reg));
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
        print(sumRegisters(HeapArrayRegister, TemporaryRegister));
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

        TreeNode *condition = node->child[0];
        TreeNode *body = node->child[1];

        printLabelNop(while_label);
        setDebugName(while_label);

        generateCodeForBranch( // If cond, then go to do_label
            do_label,
            translateCondition(condition->attr.op),
            condition);

        generateCodeForBranch( // Else, go to while_end_label
            while_end_label,
            AL);

        printLabelNop(do_label); // Goes here if cond is true

        generateCode(body);

        generateCodeForBranch(while_label, AL);
        setDebugName("Return to " + while_label);

        printLabelNop(while_end_label);
        setDebugName(while_label + " end");
    }
    break;

    case IfK:
    {
        if (node->child[2])
            generateCodeForIfElse(node);
        else
            generateCodeForIf(node);
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
        if ( // Don't declare  library functions
            (FunctionName != "fun_input") &&
            (FunctionName != "fun_output") &&
            (FunctionName != "fun_readFromMemory") &&
            (FunctionName != "fun_writeIntoMemory") &&
            (FunctionName != "fun_extractFirstHW") &&
            (FunctionName != "fun_extractSecondHW") &&
            (FunctionName != "fun_mod"))
        {
            if (shouldShowVisitingMessages)
                hr(node->attr.name);

            TreeNode *functionBody = node->child[1];

            Instruction *labelInstruction = pushRegister(ReturnAddressRegister);
            registerLabelInstruction(FunctionName, labelInstruction);

            print(labelInstruction);
            setDebugName("begin FunDeclK " + FunctionName);
            print(copySP(FramePointer));
            if (functionBody)
                generateCode(functionBody);

            DestroyARAndExitFunction(node);
            setDebugName("end FunDeclK " + FunctionName);
        }
    }
    break;

    case FunActiveK:
    {
        std::string FunctionName = std::string(node->attr.name);
        TreeNode *arg = node->child[0];
        if (FunctionName == "fun_input")
        {
            generateCode(arg);
            if (!isOS && !isBios)
                print(interrupt(1));
            print(pause());
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
            generateCode(arg);
            if (!isOS && !isBios)
                print(interrupt(1));
            printRegister(AcumulatorRegister);
            setDebugName("OUTPUT");
        }
        else if (FunctionName == "fun_readFromMemory")
        {
            generateCode(arg);
            print(
                new TypeAInstruction(49,
                                     "LDR",
                                     0,
                                     AcumulatorRegister,
                                     AcumulatorRegister));
            setDebugName("READ MEMORY");
        }
        else if (FunctionName == "fun_writeIntoMemory")
        {
            generateCodeForAnyNode(arg);
            print(pushAcumulator()); //Push data arg
            arg = arg->sibling;
            generateCodeForAnyNode(arg);           //Address in Acumulator
            generateCodeForPop(TemporaryRegister); //Data in Temporary Register
            print(
                new TypeAInstruction(
                    48,
                    "STR",
                    0,
                    AcumulatorRegister, //Address
                    TemporaryRegister   //Data
                    ));
            setDebugName("WRITE MEMORY");
        }
        else if (FunctionName == "fun_extractFirstHW")
        {
            generateCode(arg);
            print(rightShiftImmediate(AcumulatorRegister, 16));
        }
        else if (FunctionName == "fun_extractSecondHW")
        {
            generateCode(arg);
            print(extendZero(AcumulatorRegister));
        }
        else if (FunctionName == "fun_mod")
        {
            generateCodeForAnyNode(arg);
            print(pushAcumulator()); //Push b arg
            arg = arg->sibling;
            generateCodeForAnyNode(arg);           //a in Acumulator
            generateCodeForPop(TemporaryRegister); //b in Temporary Register
            print(new TypeEInstruction(65, "MOD", TemporaryRegister, AcumulatorRegister));
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
            int offset = fetchVarOffsetAsInteger(varToBeAssignedInto);
            Registers scopeRegister = varToBeAssignedInto->scope == "global"
                                          ? GlobalPointer
                                          : FramePointer;
            if (offset < 31)
                print(
                    new TypeAInstruction(48,
                                         "STR",
                                         offset,
                                         scopeRegister,
                                         AcumulatorRegister));
            else
            {
                generateCodeForConst(offset, TemporaryRegister);
                print(
                    new TypeBInstruction(40,
                                         "STR",
                                         TemporaryRegister,
                                         scopeRegister,
                                         AcumulatorRegister));
            }
        }
        break;
        case VetK:
        {
            std::cout << "VECTOR\n";
            generateCode(node->child[1]); // Value to be assigned
            print(pushAcumulator());
            generateCode(varToBeAssignedInto->child[0]);          // Offset
            loadVariable(varToBeAssignedInto, TemporaryRegister); // Base Address
            print(sumRegisters(TemporaryRegister, AcumulatorRegister));
            generateCodeForPop(AcumulatorRegister);
            print(
                new TypeAInstruction(
                    48,
                    "STR",
                    0,
                    TemporaryRegister,
                    AcumulatorRegister));
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
        generateOptimizedOperation(node);
    }
    break;
    case ConstK:
    {
        generateCodeForConst(node->attr.val, AcumulatorRegister);
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

void CodeGenerator::generateOptimizedOperation(TreeNode *node)
{
    TreeNode *rightSon = node->child[1];
    if (rightSon->nodekind != ExpK ||
        rightSon->kind.stmt != ConstK ||
        rightSon->attr.val > 255)
    {
        generateRegisterOperation(node);
        return;
    }
    generateCode(node->child[0]);
    switch (node->attr.op)
    {
    case PLUS:
        print(addImmediate(AcumulatorRegister, rightSon->attr.val));
        break;
    case MINUS:
        print(subtractImmediate(AcumulatorRegister, rightSon->attr.val));
        break;
    default:
        generateRegisterOperation(node);
        return;
    }
    setDebugName("OPTIMIZED OPERATION");
}

void CodeGenerator::generateRegisterOperation(TreeNode *node)
{
    generateCode(node->child[1]);
    print(pushAcumulator());
    generateCode(node->child[0]);
    generateCodeForPop(TemporaryRegister);
    generateOperationCode(node);
    setDebugName("Register Operation");
}

void CodeGenerator::generateOperationCode(TreeNode *node)
{
    if (node == NULL)
        return;

    if (shouldShowVisitingMessages)
        printNode(node); //Check visited node
    switch (node->attr.op)
    {
    case PLUS:
        print(sumRegisters(AcumulatorRegister, TemporaryRegister));
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
    print(nop());
    setDebugName("begin Header");
    if (isOS)
        createOSHeader();

    generateRunTimeSystem();

    if (shouldShowVisitingMessages)
        hr("Code above me is header code");
    setDebugName("end Header");
}

void CodeGenerator::createOSHeader()
{
    print(new TypeEInstruction(58, "CPXR", 0, StoredSpecReg));
    setDebugName("Create OS header");
    print(pushRegister(HeapArrayRegister));
    print(pushAcumulator());
    print(pushRegister(TemporaryRegister));
    print(pushRegister(FramePointer));
    print(pushRegister(GlobalPointer));
    print(pushRegister(ReturnAddressRegister));
    print(pushRegister(SnapshotPointer));                  // SP
    print(moveHighToLow(TemporaryRegister, LinkRegister)); // PC
    print(pushRegister(TemporaryRegister));
    print(nop());
    print(moveHighToLow(TemporaryRegister, StoredSpecReg));
    print(pushRegister(TemporaryRegister)); //SpecReg
    print(pushRegister(SystemCallRegister));
    print(copySP(TemporaryRegister));
    setDebugName("OS HEADER END");
}

void CodeGenerator::createFooter()
{
    if (shouldShowVisitingMessages)
        std::cout << "This is a footer\n";
    if (!isOS)
        return;
    generateCodeForPop(SystemCallRegister);
    print(loadImediateToRegister(SystemCallRegister, 0));
    generateCodeForPop(TemporaryRegister);
    print(moveLowToHigh(TemporaryRegister, StoredSpecReg));
    generateCodeForPop(TemporaryRegister);
    print(moveLowToHigh(TemporaryRegister, LinkRegister));
    generateCodeForPop(SnapshotPointer);
    generateCodeForPop(ReturnAddressRegister);
    generateCodeForPop(GlobalPointer);
    generateCodeForPop(FramePointer);
    generateCodeForPop(TemporaryRegister);
    generateCodeForPop(AcumulatorRegister);
    generateCodeForPop(HeapArrayRegister);
    print(new TypeEInstruction(76, "PXR", 0, StoredSpecReg));
}

void CodeGenerator::loadVariable(TreeNode *node, Registers reg)
{
    int offset = fetchVarOffsetAsInteger(node);
    Registers scopeRegister = node->scope == "global"
                                  ? GlobalPointer
                                  : FramePointer;
    if (offset < 31)
        print(
            new TypeAInstruction(
                49,
                "LDR",
                offset,
                scopeRegister,
                reg));
    else
    {
        generateCodeForConst(offset, reg);
        print(
            new TypeBInstruction(
                44,
                "LDR",
                reg,
                scopeRegister,
                reg));
    }
}

void CodeGenerator::fetchVarOffset(TreeNode *node, Registers reg)
{
    BucketList record = getRecordFromSymbolTable(node);
    print(loadImediateToRegister(reg, record->memloc));
}

int CodeGenerator::fetchVarOffsetAsInteger(TreeNode *node)
{
    BucketList record = getRecordFromSymbolTable(node);
    return record ? record->memloc : -1;
}

int CodeGenerator::fetchVarOffsetByName(std::string variable, std::string scope)
{
    TreeNode *node = newExpNode(IdK);
    node->attr.name = variable;
    node->scope = scope;
    return fetchVarOffsetAsInteger(node);
}

void hr(std::string middle)
{
    std::string rule = "-----------------------------";
    std::cout << rule << " FUNCTION " + middle + " " << rule << "\n";
}

void CodeGenerator::DestroyARAndExitFunction(TreeNode *node)
{
    DataSection ds;
    int functionVariables = ds.getSize(node->attr.name);

    //Return redirects to here
    std::string label = "end_" + node->attr.name;
    printLabelNop(label);

    generateCodeForPop(ReturnAddressRegister);
    for (int recordInAR = 0; recordInAR < functionVariables; recordInAR++)
        generateCodeForPop(TemporaryRegister);

    generateCodeForPop(FramePointer);

    print(jumpToRegister(ReturnAddressRegister));
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
    print(loadImediateToRegister(AcumulatorRegister, 0));
    setDebugName("begin GlobalAR");
    for (int i = 0; i < globalCount + 1; i++)
        print(pushAcumulator());

    print(copySP(GlobalPointer));

    if (isOS)
        setOSVariables();
    setDebugName("end GlobalAR");
}

void CodeGenerator::setOSVariables()
{
    int state = fetchVarOffsetByName("registers", "global");
    std::cout << "STATE: " << std::to_string(state) << " \n";
    if (state > 0)
    {
        print(
            new TypeAInstruction(
                48,
                "STR",
                state,
                GlobalPointer,
                TemporaryRegister));
        setDebugName("SET OS VARIABLE");
    }
}

void CodeGenerator::generateCodeForFunctionActivation(TreeNode *node)
{
    DataSection ds;
    std::string FunctionName = node->attr.name;
    int functionVariables = ds.getSize(FunctionName);
    int argumentCount = node->attr.val;

    print(pushRegister(FramePointer));
    setDebugName("begin Activation " + FunctionName);

    buildAR(functionVariables - argumentCount, argumentCount, node->child[0]);
    jumpAndLink(FunctionName);
    setDebugName("end Activation " + FunctionName);
}

void CodeGenerator::buildAR(int localVariableCount, int argumentCount, TreeNode *argumentNode)
{
    if (localVariableCount > 0)
    {
        print(loadImediateToRegister(AcumulatorRegister, 0));

        // Inserting the local vars into the AR
        for (int i = 0; i < localVariableCount; ++i)
            print(pushAcumulator());
    }

    pushArguments(argumentCount, argumentNode);
}

void CodeGenerator::pushArguments(int argumentCount, TreeNode *argumentNode)
{
    for (int i = 0; i < argumentCount; i++)
    {
        generateCodeForAnyNode(argumentNode);
        print(pushAcumulator());
        argumentNode = argumentNode->sibling;
    }
}

void CodeGenerator::jumpAndLink(std::string FunctionName)
{
    int branchCodeStart = code.size();
    Instruction *jumpAndLinkAddress = sumWithPC(ReturnAddressRegister, 0);
    print(jumpAndLinkAddress);

    generateCodeForBranch(FunctionName, AL);

    jumpAndLinkAddress->immediate = code.size() - branchCodeStart;
}

void CodeGenerator::generateRunTimeSystem()
{
    generateGlobalAR();
    generateCodeForFunctionActivation(mainActivation);
    destroyGlobalAR();
    createFooter();
    print(interrupt(isBios ? 4 : 2));
}

void CodeGenerator::destroyGlobalAR()
{
    DataSection ds;
    int globalCount = ds.getSize("global");
    for (int i = 0; i < globalCount + 1; i++)
        generateCodeForPop(TemporaryRegister);
}

void CodeGenerator::generateBinaryCode(std::string outputFile)
{
    printf("\n\n +++++ Code generator! +++++ \n\n");

    mif.open(outputFile, isBios, isCompressedProgram);

    if (programOffset)
        generateCodeToJumpToOS();

    if (isCompressedProgram)
        mountFileStructure();
    else
        mountUncompressedProgram();

    mif.printMultipleEmptyPosition(memorySize);

    mif.printFooter();
    printf("\n\n Output saved on %s \n\n", outputFile.c_str());
}

void CodeGenerator::mountFileStructure()
{
    std::cout << "Compressed File\n";
    int headerSize = 1;
    int slotStart = 2060;

    mif.printSize(code.size(), slotStart);

    for (int i = 0; i < (int)code.size(); i += 2)
    {
        bool hasNext = (i + 1) < (int)code.size();
        std::string leftInstr = code[i]->to_binary();
        std::string rightInstr = hasNext
                                     ? code[i + 1]->to_binary()
                                     : "0000000000000000";
        std::string debugText = code[i]->to_string();
        if (hasNext)
            debugText += " | " + code[i + 1]->to_string();

        mif.printInstruction(slotStart + headerSize + ((int)i / 2),
                             leftInstr + rightInstr,
                             debugText);

        if (!code[i]->debugname.empty())
            mif.printDebugMsg(code[i]->debugname);
        if (hasNext && (!code[i + 1]->debugname.empty()))
            mif.printDebugMsg(code[i + 1]->debugname);

        mif.jumpLine();
    }
}

void CodeGenerator::mountUncompressedProgram()
{
    if (isOS)
    {
        mif.printOSSize(code.size(), programOffset);
        programOffset++;
    }
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
}

void CodeGenerator::generateCodeToJumpToOS()
{
    std::vector<Instruction *> originalCode = code, newCode;
    std::string originalInst = generatedCode, ngc;
    code = newCode;
    generatedCode = ngc;
    generateCodeForConst(programOffset, AcumulatorRegister);

    print(jumpToRegister(AcumulatorRegister));

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

    mif.printMultipleEmptyPosition(programOffset);
    code = originalCode;
    generatedCode = originalInst;
}

void CodeGenerator::generateCodeForConst(int value, Registers reg)
{
    Bytes number = Bytes(value);
    int nulls = 0;
    int current = number.findFirstByteIndex();
    print(loadImediateToRegister(reg, number.getNthByte(current)));
    setDebugName("begin ConstK");
    for (int i = current + 1; i < 4; i++)
    {
        int b = number.getNthByte(i);
        if (b == 0)
            nulls++;
        else
        {
            print(leftShiftImmediate(reg,
                                     8 * (nulls + 1)));
            print(addImmediate(reg, b));
            nulls = 0;
        }
    }
    if (nulls > 0)
        print(leftShiftImmediate(reg, 8 * nulls));

    setDebugName("end ConstK");
}

void CodeGenerator::printRegister(Registers reg)
{
    print(pause());
    print(outputRegister(reg));
}
