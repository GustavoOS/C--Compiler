#ifndef _INTERMEDIATE_CODE_
#define _INTERMEDIATE_CODE_

#include <iostream>
#include <vector>

#include "globals.h"

class UtilCounter
{
  public:
    std::string getNextTemporaryVariable();
    std::string getTemporaryVariable();

    void registerVariable(std::string);

    std::string getNextLabel();

  private:
    int temporaryVariableCount = 0, labelCount = 0;
    std::string lastVariable;

    std::string formatTemporaryVariable(int);
};

class Quadruple
{
  public:
    Quadruple *withOperation(std::string);
    Quadruple *withFirstOperand(std::string);
    Quadruple *withSecondOperand(std::string);
    Quadruple *withThirdOperand(std::string);

    Quadruple *withLabel(std::string);
    std::string toString();

  private:
    std::string operation,
        operand1,
        operand2,
        operand3;
};

class IntermediateCodeGenerator
{
  public:
    void generateIntermediateCode(TreeNode *);
    void printIntermediateCode();

  private:
    void generate(TreeNode *);
    void generateForSingleNode(TreeNode *);

    UtilCounter counter;
    std::vector<Quadruple *> intermediateCode;
    int labelCount = 0;

    void generateForStmtNode(TreeNode *);
    void generateForExprNode(TreeNode *);

    // Statements
    void generateIf(TreeNode *);
    void generateWhile(TreeNode *);
    void generateFunctionCall(TreeNode *);
    void generateVectorDeclaration(TreeNode *);
    void generateFunctionDeclaration(TreeNode *);
    void generateReturn(TreeNode *);
    void generateAssignment(TreeNode *);

    // Expressions
    void generateOperation(TreeNode *);
    void generateConstant(TreeNode *);
    void generateVariableAccess(TreeNode *);
    void generateVectorAccess(TreeNode *);

    // Private methods
    void insertQuadruple(Quadruple *);
    Quadruple *newQuadruple();
    std::string translateOperation(TokenType);
    std::string getOperandName(TreeNode *);
    static void printQuadruple(Quadruple *);
};

#endif