#ifndef _CODE_GENERATOR_
#define _CODE_GENERATOR_

#include <vector>
#include <iostream>
#include "globals.h"
#include <map>
#include "mifgenerator.h"

enum Registers : int
{
  HeapArrayRegister = 0,
  AcumulatorRegister,
  TemporaryRegister,
  FramePointer,
  GlobalPointer,
  BaseAddressRegister,
  ReturnAddressRegister,
  SwapRegister = 8
};

std::string printRegister( int reg );


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
  AB,     //Absolute, always
  CS = 2, //Unsigned Greater than or equal
  CC = 3  //Unsigned Lower THan
};

ConditionCodes translateCondition(int operation);

class Instruction
{
public:
  int id;
  std::string name, label, debugname;
  int regm, regn, regd, condition, immediate, offset;
  virtual std::string to_string() = 0;
  virtual std::string to_binary() {
    return getOpCode(id);
  };

  bool isLabel = false;
  void setlabel(std::string newLabel);
  std::string to_string_with_label();

  int relativeAddress;

  static std::string getOpCode( int id );
  static std::string getOpBit( int id );
  static std::string getFunct2( int id );
  static std::string getFunct1( int id );
};

class BranchLabel;
class TypeDInstruction;

Instruction *nop();
Instruction *loadImediateToRegister(Registers regis, int number);
Instruction *pushAcumulator();
Instruction *pushRegister(Registers reg);
Instruction *popRegister(Registers reg);
Instruction *jumpToRegister(Registers reg);
Instruction *moveLowToLowRegister(Registers origin, Registers destination);
Instruction *subImeditateFromRegister(int value, Registers destination);

Instruction *moveLowToHigh(Registers low, Registers high);
Instruction *moveHighToLow(Registers low, Registers high);
Instruction * outputRegister(Registers reg);

void hr(std::string);

class CodeGenerator
{
public:
  CodeGenerator(bool displayable, int programOffset);
  void generate(TreeNode *node);
  void linker();
  void generateBinaryCode(std::string outputFile);

private:
  std::string generatedCode;
  std::vector<Instruction *> code;
  std::map<std::string, Instruction *> labelDestMap;
  std::map<std::string, std::vector< BranchLabel *> > labelOriginMap;
  bool shouldPrintGeneratedCodeOnScreen;
  bool shouldShowVisitingMessages;
  TreeNode * mainActivation;
  int programOffset;

  //Private methods
  void print(Instruction *instruction);
  void generateCode(TreeNode *node);
  void createHeader();
  void createFooter();
  void generateCodeForAnyNode(TreeNode *);
  void generateCodeForStmtNode(TreeNode *node);
  void generateCodeForExprNode(TreeNode *node);
  void generateOperationCode(TreeNode *);
  void generateCodeForBranch(std::string branch_name, ConditionCodes condition, TreeNode * child = NULL);
  void generateCodeForPop(Registers reg);
  void registerLabelInstruction(std::string label, Instruction *Instruction);
  void generateCodeForConst(int);

  void fetchVarOffset(TreeNode *node, Registers reg);
  void loadVariable(TreeNode *node, Registers reg);
  void printLabelNop(std::string);

  void setDebugName( std::string name );

  void DestroyARAndExitFunction(TreeNode *);
  void generateGlobalAR();
  void generateCodeForFunctionActivation(TreeNode *node);
  void generateRunTimeSystem();
  void destroyGlobalAR();

  void generateCodeToJumpToOS();
  MifGenerator mif;
};

class BranchLabel
{
  std::string tolabel;

public:
  Instruction *leftByte;
  Instruction *rightByte;

  BranchLabel(std::string gotolabel, Instruction *lByte, Instruction *rByte);
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
  std::string to_binary();
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
  std::string to_binary();
};

class TypeCInstruction : public Instruction
{
public:
  
  TypeCInstruction(
      int identity,
      std::string instructionName,
      int instructionImmediate,
      int RegisterN,
      int RegisterD);

  std::string to_string();

  std::string to_binary();
};

class TypeDInstruction : public Instruction
{
public:
  TypeDInstruction(
      int identity,
      std::string instructionName,
      int RegisterD,
      int instructionImmediate);

  std::string to_string();

  std::string to_binary();
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

  std::string to_binary();
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

  std::string to_binary();
};

class TypeGInstruction : public Instruction
{
public:
  TypeGInstruction(
      int identity,
      std::string instructionName,
      int conditionCode,
      int offsetSize);

  std::string to_string();
  std::string to_binary();
};





#endif