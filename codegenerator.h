#ifndef _CODE_GENERATOR_
#define _CODE_GENERATOR_

#include <vector>
#include <iostream>
#include "globals.h"

enum Registers
{
    HeapArrayRegister = 1,
    AcumulatorRegister,
    TemporaryRegister,
    FramePointer,
    GlobalPointer
};

class CodeGenerator
{
  public:
    CodeGenerator(bool displayable);
    void generate(TreeNode *node);

  private:
    std::string generatedCode;
    bool shouldPrintGeneratedCodeOnScreen;

    //Private methods
    void print(std::string code);
    void generateCode(TreeNode *node);
    void createHeader();
    void createFooter();
    void generateCodeForStmtNode(TreeNode *node);
    void generateCodeForExprNode(TreeNode *node);

};

#endif