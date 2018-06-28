#ifndef _CODE_GENERATOR_
#define _CODE_GENERATOR_

#include <vector>
#include <iostream>
#include "globals.h"
#include <vector>
#include <map>

enum Registers
{
  HeapArrayRegister = 0,
  AcumulatorRegister,
  TemporaryRegister,
  FramePointer,
  GlobalPointer,
  BaseAddressRegister,
  ReturnAddressRegister,
  SwapRegister = 0
};

enum ConditionCodes
{
  EQ = 0, //Equal
  NE,     //NOT EQUAL
  HS,     //Unsigned Greater than or equal
  LO,     //Unsigned Lower then
  MI,     //Negative
  PL,     //Positive or zero
  VS,     //Overflow
  VC,     //No overflow
  HI,     //Unsigned greater than
  LS,     //Unsgined lower than or equal
  GE,     //Signed greater than or equal
  LT,     //Signed lower than
  GT,     //Signed greater than
  LE,     //Signed lower than
  AL,     //Always
  NEVER,  //Never
  CS = 2, //Unsigned Greater than or equal
  CC = 3  //Unsigned Lower THan
};

ConditionCodes translateCondition(int operation);

class Instruction
{
public:
  int id;
  std::string name, label;
  int regm, regn, regd, condition, immediate, offset;
  virtual std::string to_string();
  bool isLabel = false;
  void setlabel(std::string newLabel);
  std::string to_string_with_label();

  int relativeAddress;
};

class BranchLabel;
class TypeDInstruction;

Instruction *nopWithLabel(std::string label);
Instruction *loadImediateToRegister(Registers regis, int number);
Instruction *pushAcumulator();
Instruction *pushRegister(int);
Instruction *popRegister(Registers reg);
Instruction *jumpToRegister(Registers reg);
Instruction *moveToLowRegister(Registers origin, Registers destination);

void hr(std::string);

class CodeGenerator
{
public:
  CodeGenerator(bool displayable);
  void generate(TreeNode *node);
  void linker();

private:
  std::string generatedCode;
  std::vector<Instruction *> code;
  std::map<std::string, Instruction *> labelDestMap;
  std::map<std::string, BranchLabel *> labelOriginMap;
  bool shouldPrintGeneratedCodeOnScreen;
  bool shouldShowVisitingMessages;

  //Private methods
  void print(Instruction *instruction);
  void generateCode(TreeNode *node);
  void createHeader();
  void createFooter();
  void generateCodeForAnyNode(TreeNode *);
  void generateCodeForStmtNode(TreeNode *node);
  void generateCodeForExprNode(TreeNode *node);
  void generateOperationCode(TreeNode *);
  void generateCodeForBranch(std::string branch_name);

  void DestroyARAndExitFunction(TreeNode *);
};

class BranchLabel
{
  std::string tolabel;

public:
  TypeDInstruction *leftByte;
  TypeDInstruction *rightByte;

  BranchLabel(std::string gotolabel, TypeDInstruction *lByte, TypeDInstruction *rByte);
  std::string to_string();
};

class TypeAInstruction : public Instruction
{
public:
  TypeAInstruction(
      int identity,
      std::string instructionName,
      int instructionImmediate,
      int RegisterM,
      int RegisterD);
  std::string to_string();
};

class TypeBInstruction : public Instruction
{
public:
  TypeBInstruction(
      int identity,
      std::string instructionName,
      int RegisterM,
      int RegisterN,
      int RegisterD);
  std::string to_string();
};

class TypeCInstruction : public Instruction
{
public:
  std::string to_string();
  TypeCInstruction(
      int identity,
      std::string instructionName,
      int instructionImmediate,
      int RegisterM,
      int RegisterD);
};

class TypeDInstruction : public Instruction
{
public:
  std::string to_string();
  TypeDInstruction(
      int identity,
      std::string instructionName,
      int RegisterD,
      int instructionImmediate);
};

class TypeEInstruction : public Instruction
{
public:
  TypeEInstruction(
      int identity,
      std::string instructionName,
      int RegisterM,
      int RegisterD);

  std::string to_string();
};

class TypeFInstruction : public Instruction
{
public:
  TypeFInstruction(
      int identity,
      std::string instructionName,
      int conditionCode,
      int RegisterD);

  std::string to_string();
};

class TypeGInstruction : public Instruction
{
public:
  TypeGInstruction(
      int identity,
      std::string instructionName,
      int conditionCode,
      int offsetSize,
      std::string labelTo);

  std::string to_string();
};

#endif