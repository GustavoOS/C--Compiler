#ifndef _INSTRUCTIONS_
#define _INSTRUCTIONS_

#include <iostream>

enum Registers : int
{
    HeapArrayRegister = 0,
    AcumulatorRegister,
    TemporaryRegister,
    FramePointer,
    GlobalPointer,
    SnapshotPointer,
    ReturnAddressRegister,
    SystemCallRegister,
    StoredSpecReg,
    LinkRegister = 13,
    StackPointer,
    ProgramCounter
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
    AB,     //Absolute, always
    CS = 2, //Unsigned Greater than or equal
    CC = 3  //Unsigned Lower THan
};

std::string printRegister(int reg);

class Instruction
{
public:
    int id;
    std::string name, label, debugname;
    int regm, regn, regd, condition, immediate, offset;
    virtual std::string to_string() = 0;
    virtual std::string to_binary()
    {
        return getOpCode(id);
    };

    bool isLabel = false;
    void setlabel(std::string newLabel);
    std::string to_string_with_label();

    int relativeAddress;

    static std::string getOpCode(int id);
    static std::string getOpBit(int id);
    static std::string getFunct2(int id);
    static std::string getFunct1(int id);
};

Instruction *nop();
Instruction *loadImediateToRegister(Registers regis, int number);
Instruction *pushAcumulator();
Instruction *pushRegister(Registers reg);
Instruction *popRegister(Registers reg);
Instruction *jumpToRegister(Registers reg);
Instruction *moveLowToLowRegister(Registers origin, Registers destination);
Instruction *subImeditateFromRegister(int value, Registers destination);
Instruction *sumWithPC(Registers reg, int number);
Instruction *sumRegisters(Registers ra, Registers rb); // A = A + B
Instruction *copySP(Registers reg);
Instruction *extendZero(Registers reg);
Instruction *rightShiftImmediate(Registers, int);
Instruction *branchImmediate(ConditionCodes, int);
Instruction *leftShiftImmediate(Registers, int);
Instruction *addImmediate(Registers, int);
Instruction *subtractImmediate(Registers, int);
Instruction *signExtendHW(Registers);
Instruction *interrupt();

Instruction *moveLowToHigh(Registers low, Registers high);
Instruction *moveHighToLow(Registers low, Registers high);
Instruction *outputRegister(Registers reg);
Instruction *halt();
Instruction *pause();

class BranchLabel;
class TypeDInstruction;

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

class BranchLabel
{
    std::string tolabel;

public:
    Instruction *firstByte;
    Instruction *secondByte;
    Instruction *branch;

    BranchLabel(std::string gotolabe, ConditionCodes condition);

    std::string to_string();
};

#endif
