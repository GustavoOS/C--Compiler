#ifndef _CODE_GENERATOR_
#define _CODE_GENERATOR_

#include <vector>
#include <iostream>
#include "globals.h"
#include <map>
#include "mifgenerator.h"
#include "instructions.h"

ConditionCodes translateCondition(int operation);

void hr(std::string);

class CodeGenerator
{
public:
    CodeGenerator(bool displayable, int programOffset);
    void setMode(bool isBios, bool isCompressed, bool isOS);
    void generate(TreeNode *node);
    void linker();
    void generateBinaryCode(std::string outputFile);

private:
    std::string generatedCode;
    std::vector<Instruction *> code;
    std::map<std::string, Instruction *> labelDestMap;
    std::map<std::string, std::vector<BranchLabel *>> labelOriginMap;
    bool shouldPrintGeneratedCodeOnScreen;
    bool shouldShowVisitingMessages;
    TreeNode *mainActivation;
    int programOffset;
    MifGenerator mif;
    bool isBios, isCompressedProgram, isOS;
    int memorySize;

    //Private methods
    void print(Instruction *instruction);
    void generateCode(TreeNode *node);
    void createHeader();
    void createFooter();
    void generateCodeForAnyNode(TreeNode *);
    void generateCodeForStmtNode(TreeNode *node);
    void generateCodeForExprNode(TreeNode *node);
    void generateOperationCode(TreeNode *);
    void generateCodeForBranch(std::string branch_name, ConditionCodes condition, TreeNode *child = NULL);
    void generateCodeForIf(TreeNode * node);
    void generateCodeForIfElse(TreeNode * node);
    void generateCodeForPop(Registers reg);
    void registerLabelInstruction(std::string label, Instruction *Instruction);
    void generateCodeForConst(int);

    void mountFileStructure();
    void mountUncompressedProgram();

    void fetchVarOffset(TreeNode *node, Registers reg);
    void loadVariable(TreeNode *node, Registers reg);
    void printLabelNop(std::string);

    void setDebugName(std::string name);

    void DestroyARAndExitFunction(TreeNode *);
    void generateGlobalAR();
    void generateCodeForFunctionActivation(TreeNode *node);
    void generateRunTimeSystem();
    void destroyGlobalAR();
    void buildAR(int localVariableCount, int argumentCount, TreeNode *argumentNode);
    void jumpAndLink(std::string);
    void goToApplication();

    void insertIndexInsideEveryInstruction();
    void printEveryLabelLink();
    void generateCodeToJumpToOS();

    void printRegister(Registers reg);
    void pushArguments(int argumentCount, TreeNode *argumentNode);
};

#endif
