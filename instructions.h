#ifndef _INSTRUCTIONS_
#define _INSTRUCTIONS_

#include <iostream>

enum Registers : int
{
    HeapArrayRegister = 0, /* Low Registers */
    AcumulatorRegister,
    TemporaryRegister,
    SecondRegister,
    FramePointer,
    GlobalPointer,
    UserSPKeeper,
    SystemCallRegister, /* Low registers*/
    StoredSpecReg, /* High Registers */
    LinkRegister = 12,
    PCKeeper,
    StackPointer,
    ProgramCounter /* High Registers */
};

enum ConditionCodes
{
    EQ = 0, //Equal
    NE,     //NOT EQUAL
    HS,     //Unsigned Greater than or equal
    LO,     //Unsigned less then
    MI,     //Negative
    PL,     //Positive or zero
    VS,     //Overflow (V set)
    VC,     //No overflow (V Clear)
    HI,     //Unsigned greater than
    LS,     //Unsgined less than or equal
    GE,     //Signed greater than or equal
    LT,     //Signed less than
    GT,     //Signed greater than
    LE,     //Signed less than or equal
    AL,     //Always
    CS = 2, //Unsigned Greater than or equal
    CC = 3  //Unsigned less THan
};

enum SystemCalls
{
    StandardPreemptionFlow = 0,
    IORequest,
    ProgramCompletion,
    UserPreemption,
    BiosCompletion
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
    int debugAddress;
    
    static std::string getOpCode(int id);
    static std::string getOpBit(int id);
    static std::string getFunct2(int id);
    static std::string getFunct1(int id);
    static std::string getConditionString(ConditionCodes condition);
};

Instruction *nop();
Instruction *loadImmediateToRegister(Registers regis, int number);
Instruction *pushAcumulator();
Instruction *pushRegister(Registers reg);
Instruction *pushMultiple(int count);
Instruction *popMultiple(int count);
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
Instruction *branchLink(Registers reg);
Instruction *relativeBranch(ConditionCodes cond, Registers reg);
Instruction *leftShiftImmediate(Registers, int);
Instruction *addImmediate(Registers, int);
Instruction *subtractImmediate(Registers, int);
Instruction *signExtendHW(Registers);
Instruction *interrupt(SystemCalls systemCall);
Instruction *compare(Registers a, Registers b);
Instruction *storeWithImmediate(Registers content, Registers baseAddr, int offset);
Instruction *storeWithRegister(Registers content, Registers baseAddr, Registers offset);
Instruction *loadWithImmediate(Registers target, Registers baseAddr, int offset);
Instruction *loadWithRegister(Registers target, Registers baseAddr, Registers offset);

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

class TypeHInstruction : public Instruction
{
public:
    TypeHInstruction(
        int identity,
        std::string instructionName,
        int RegisterD);

    std::string to_string();

    std::string to_binary();
};

class TypeIInstruction : public Instruction
{
public:
    TypeIInstruction(
        int identity,
        std::string instructionName,
        int value);

    std::string to_string();

    std::string to_binary();
};

class TypeJInstruction : public Instruction
{
public:
    TypeJInstruction(
        int identity,
        std::string instructionName,
        int regd);

    std::string to_string();

    std::string to_binary();
};

class TypeKInstruction : public Instruction
{
public:
    TypeKInstruction(
        int identity,
        std::string instructionName,
        int regd);

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

    BranchLabel(std::string gotolabe, ConditionCodes condition, bool isJumpAndLink);

    std::string to_string();
};

#endif 
