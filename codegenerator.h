#ifndef _CODE_GENERATOR_
#define _CODE_GENERATOR_

#include <vector>
#include <iostream>
#include "globals.h"

class CodeGenerator
{
  public:
    CodeGenerator(bool displayable);
    void generate(TreeNode *node);

  private:
    bool shouldPrintGeneratedCodeOnScreen;
    std::string generatedCode;
    void print(std::string code);
    void generateCodeForStmtNode(TreeNode * node);
    void generateCodeForExprNode(TreeNode *node);
};

#endif