#ifndef _CODE_GENERATOR_
#define _CODE_GENERATOR_

#include <vector>
#include <iostream>
#include "globals.h"
#include <vector>

enum Registers
{
  HeapArrayRegister = 1,
  AcumulatorRegister,
  TemporaryRegister,
  FramePointer,
  GlobalPointer
};

class Instruction
{
public:
  int id;
  std::string name, label;
  int regm, regn, regd, condition, immediate, offset;
  virtual std::string to_string();
};

class CodeGenerator
{
public:
  CodeGenerator(bool displayable);
  void generate(TreeNode *node);

private:
  std::string generatedCode;
  std::vector<Instruction> code;
  bool shouldPrintGeneratedCodeOnScreen;

  //Private methods
  void print(Instruction *instruction);
  void generateCode(TreeNode *node);
  void createHeader();
  void createFooter();
  void generateCodeForStmtNode(TreeNode *node);
  void generateCodeForExprNode(TreeNode *node);
};

class TypeAInstruction : public Instruction
{
public:
  TypeAInstruction(
      int identity,
      std::string instructionName,
      int RegisterM,
      int RegisterN,
      int RegisterD);
  std::string to_string();
};

class TypeBInstruction : public Instruction
{
  TypeBInstruction(
      int identity,
      std::string instructionName,
      int RegisterM,
      int RegisterN,
      int RegisterD);
  std::string to_string();
};

// class TypeCInstruction : public Instruction
// {
//   std::string to_string();
// };

#endif