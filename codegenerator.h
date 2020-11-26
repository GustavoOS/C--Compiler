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
    CodeGenerator(bool displayable);
    void setMode(bool isBios, bool isOS);
    void generate(TreeNode *node);
    void generateCodeToJumpToOS();
    void linker();
    std::vector<Instruction *> getCode()
    {
        return code;
    }

    std::map<std::string, Instruction *> getDestMap()
    {
        return labelDestMap;
    }
    std::map<std::string, std::vector<BranchLabel *>> getOriginMap()
    {
        return labelOriginMap;
    }

private:
    std::string generatedCode;
    std::vector<Instruction *> code;
    std::map<std::string, Instruction *> labelDestMap;
    std::map<std::string, std::vector<BranchLabel *>> labelOriginMap;
    bool shouldPrintGeneratedCodeOnScreen;
    bool shouldShowVisitingMessages;
    TreeNode *mainActivation;
    bool isBios, isOS;

    //Private methods
    void print(Instruction *instruction);
    void generateCode(TreeNode *node);
    void createHeader();
    void createOSHeader();
    void createBIOSHeader();
    void createFooter();
    void generateCodeForAnyNode(TreeNode *);
    void generateCodeForStmtNode(TreeNode *node);
    void generateCodeForExprNode(TreeNode *node);
    void generateOperationCode(TreeNode *);
    void generateRegisterOperation(TreeNode *);
    void generateOptimizedOperation(TreeNode *);
    void generateCodeForBranch(std::string branch_name, ConditionCodes condition, TreeNode *child = NULL, bool isJumpAndLink = false);
    void generateCodeForIf(TreeNode *node);
    void generateCodeForIfElse(TreeNode *node);
    void generateCodeForPop(Registers reg);
    void registerLabelInstruction(std::string label, Instruction *Instruction);
    void generateCodeForConst(int, Registers);

    int fetchVarOffsetAsInteger(TreeNode *node);
    int fetchVarOffsetByName(std::string variable, std::string scope);
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

    void printRegister(Registers reg);
    void pushArguments(int argumentCount, TreeNode *argumentNode);

    void generateCodeForSimpleVariableAssignment(TreeNode * variable, TreeNode * value);

    void pushMultipleIfNeeded(int number);
    void popMultipleIfNeeded(int number);

    SystemCalls getFooterSystemCall();
};

#endif
