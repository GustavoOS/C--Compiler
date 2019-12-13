#include "instructions.h"
#include <bitset>
#include <vector>
#include <map>

//Instruction Class
std::string Instruction::to_string()
{
    return "Generic Instruction";
}

Instruction *jumpToRegister(Registers reg)
{
    return new TypeFInstruction(
        38,
        "BX",
        AL,
        reg);
}
Instruction *outputRegister(Registers reg)
{
    return new TypeEInstruction(
        69,
        "OUTPUT",
        0,
        reg);
}

Instruction *
popRegister(Registers reg)
{
    return new TypeEInstruction(
        68,
        "POP",
        0,
        reg);
}

Instruction *pushRegister(Registers reg)
{
    return new TypeEInstruction(
        67,
        "PUSH",
        0,
        reg);
}

Instruction *loadImediateToRegister(Registers regis, int number)
{
    return new TypeDInstruction(
        8,
        "MOV",
        regis,
        number);
}

Instruction *pushAcumulator()
{
    return pushRegister(AcumulatorRegister);
}

Instruction *nop()
{
    return new TypeDInstruction(74, "NOP", 0, 0);
}

Instruction *moveLowToLowRegister(Registers origin, Registers destination)
{
    return new TypeCInstruction(
        6,
        "ADD",
        0,
        origin,
        destination);
}

Instruction *subImeditateFromRegister(int value, Registers destination)
{
    // return new TypeDInstruction(
    //     11,
    //     "SUB",
    //     destination,
    //     value
    // );
    return new TypeCInstruction(
        7,
        "SUB",
        value,
        destination,
        destination);
}

Instruction *moveLowToHigh(Registers low, Registers high)
{
    return new TypeEInstruction(
        36,
        "MOV",
        low,
        high);
}

Instruction *moveHighToLow(Registers low, Registers high)
{
    return new TypeEInstruction(
        35,
        "MOV",
        high,
        low);
}

std::string getVal5Bits(int val)
{
    std::bitset<5> bs(val);
    return bs.to_string();
}

std::string getVal3Bits(int val)
{
    std::bitset<3> bs(val);
    return bs.to_string();
}

std::string getVal4Bits(int val)
{
    std::bitset<4> bs(val);
    return bs.to_string();
}

std::string getVal8Bits(int val)
{
    std::bitset<8> bs(val);
    return bs.to_string();
}

std::string getVal8BitsSignal(int val)
{
    std::bitset<8> bs(val);
    // TODO : Add support to signal
    return bs.to_string();
}

void Instruction::setlabel(std::string newLabel)
{
    isLabel = true;
    label = newLabel;
}

std::string Instruction::to_string_with_label()
{
    return isLabel ? label + ": " + this->to_string()
                   : this->to_string();
}

BranchLabel::BranchLabel(std::string gotolabel, Instruction *lByte, Instruction *rByte)
{
    tolabel = gotolabel;
    leftByte = lByte;
    rightByte = rByte;
}

std::string BranchLabel::to_string()
{
    return "Branch to " + tolabel;
}

TypeAInstruction::TypeAInstruction(
    int identity,
    std::string instructionName,
    int instructionImmediate,
    int RegisterM,
    int RegisterD)
{
    id = identity;
    name = instructionName;
    immediate = instructionImmediate;
    regm = RegisterM;
    regd = RegisterD;
}

std::string TypeAInstruction::to_string()
{
    return "(" + std::to_string(id) + ") " +
           name + " " + std::to_string(immediate) + " " +
           printRegister(regm) + " " + printRegister(regd);
};

std::string TypeAInstruction::to_binary()
{
    return getOpCode(id) + getOpBit(id) + getVal5Bits(immediate) + getVal3Bits(regm) + getVal3Bits(regd);
}

TypeBInstruction::TypeBInstruction(
    int identity,
    std::string instructionName,
    int RegisterM,
    int RegisterN,
    int RegisterD)
{
    id = identity;
    name = instructionName;
    regn = RegisterN;
    regm = RegisterM;
    regd = RegisterD;
}
std::string TypeBInstruction::to_string()
{
    return "(" + std::to_string(id) + ") " +
           name + " " + printRegister(regm) + " " +
           printRegister(regn) + " " +
           printRegister(regd);
}

std::string TypeBInstruction::to_binary()
{
    return getOpCode(id) + getOpBit(id) + getFunct1(id) + getVal3Bits(regm) + getVal3Bits(regn) + getVal3Bits(regd);
}

TypeCInstruction::TypeCInstruction(
    int identity,
    std::string instructionName,
    int instructionImmediate,
    int RegisterN,
    int RegisterD)
{
    id = identity;
    name = instructionName;
    immediate = instructionImmediate;
    regn = RegisterN;
    regd = RegisterD;
}

std::string TypeCInstruction::to_string()
{
    return "(" + std::to_string(id) + ") " +
           name + " " + std::to_string(immediate) + " " +
           printRegister(regn) + " " + printRegister(regd);
}

std::string TypeCInstruction::to_binary()
{
    return getOpCode(id) + getOpBit(id) + getFunct1(id) + getVal3Bits(immediate) + getVal3Bits(regn) + getVal3Bits(regd);
}

std::string TypeDInstruction::to_string()
{
    return "(" + std::to_string(id) + ") " +
           name + " " + printRegister(regd) + " " +
           std::to_string(immediate);
}

std::string TypeDInstruction::to_binary()
{
    return getOpCode(id) + getOpBit(id) + getVal3Bits(regd) + getVal8Bits(immediate);
}

TypeDInstruction::TypeDInstruction(
    int identity,
    std::string instructionName,
    int RegisterD,
    int instructionImmediate)
{
    id = identity;
    name = instructionName;
    immediate = instructionImmediate;
    regd = RegisterD;
}

TypeEInstruction::TypeEInstruction(
    int identity,
    std::string instructionName,
    int RegisterM,
    int RegisterD)
{
    id = identity;
    name = instructionName;
    regm = RegisterM;
    regd = RegisterD;
}

std::string TypeEInstruction::to_string()
{
    return "(" + std::to_string(id) + ") " +
           name + " " + printRegister(regm) + " " +
           printRegister(regd);
}

std::string TypeEInstruction::to_binary()
{
    return getOpCode(id) + getFunct2(id) + getFunct1(id) + getVal3Bits(regm) + getVal3Bits(regd);
}

TypeFInstruction::TypeFInstruction(
    int identity,
    std::string instructionName,
    int conditionCode,
    int RegisterD)
{
    id = identity;
    name = instructionName;
    condition = conditionCode;
    regd = RegisterD;
}

std::string TypeFInstruction::to_string()
{
    return "(" + std::to_string(id) + ") " +
           name + " " + std::to_string(condition) + " " +
           printRegister(regd);
}

std::string TypeFInstruction::to_binary()
{
    return getOpCode(id) + getFunct2(id) + getVal4Bits(condition) + getVal4Bits(regd);
}

TypeGInstruction::TypeGInstruction(
    int identity,
    std::string instructionName,
    int conditionCode,
    int offsetSize)
{
    id = identity;
    name = instructionName;
    condition = conditionCode;
    offset = offsetSize;
}

std::string TypeGInstruction::to_string()
{
    return "(" + std::to_string(id) + ") " +
           name + " " + std::to_string(condition) + " " +
           std::to_string(offset) + " :" + label;
}

std::string TypeGInstruction::to_binary()
{
    return getOpCode(id) + getVal4Bits(condition) + getVal8BitsSignal(offset);
}

std::vector<std::string> opcode =
    {
        "0000", // 0
        "0000", // 1
        "0001", // 2
        "0001", // 3
        "0001", // 4
        "0001", // 5
        "0001", // 6
        "0010", // 7
        "0010", // 8
        "0011", // 9
        "0011", // 10
        "0100", // 12
        "0100", // 13
        "0100", // 14
        "0100", // 15
        "0100", // 16
        "0100", // 17
        "0100", // 18
        "0100", // 19
        "0100", // 20
        "0100", // 21
        "0100", // 22
        "0100", // 23
        "0100", // 24
        "0100", // 25
        "0100", // 26
        "0100", // 27
        "0100", // 28
        "0100", // 29
        "0100", // 30
        "0100", // 31
        "0100", // 32
        "0100", // 33
        "0100", // 34
        "0100", // 35
        "0100", // 36
        "0100", // 37
        "0100", // 38
        "0100", // 39
        "0101", // 40
        "0101", // 41
        "0101", // 42
        "0101", // 43
        "0101", // 44
        "0101", // 45
        "0101", // 46
        "0101", // 47
        "0110", // 48
        "0110", // 49
        "0111", // 50
        "0111", // 51
        "1000", // 52
        "1000", // 53
        "1001", // 54
        "1001", // 55
        "1010", // 56
        "1010", // 57
        "1011", // 58
        "1011", // 59
        "1011", // 60
        "1011", // 61
        "1011", // 62
        "1011", // 63
        "1011", // 64
        "1011", // 65
        "1011", // 66
        "1011", // 67
        "1011", // 68
        "1011", // 69
        "1011", // 70
        "1011", // 71
        "1100", // 72
        "1101", // 73
        "1110", // 74
        "1110", // 75
};

std::map<int, int> opMap = {
    {1, 0},
    {2, 1},
    {4, 1},
    {3, 0},
    {5, 1},
    {6, 1},
    {7, 1},
    {8, 0},
    {9, 1},
    {10, 0},
    {11, 1},
    {39, 1},
    {40, 0},
    {41, 0},
    {42, 0},
    {43, 0},
    {44, 1},
    {45, 1},
    {46, 1},
    {47, 1},
    {48, 0},
    {49, 1},
    {50, 0},
    {51, 1},
    {52, 0},
    {53, 1},
    {54, 0},
    {55, 1},
    {56, 0},
    {57, 1},
    {39, 1},
    {40, 0},
    {41, 0},
    {42, 0},
    {43, 0},
    {44, 1},
    {45, 1},
    {46, 1},
    {47, 1},
    {48, 0},
    {49, 1},
    {50, 0},
    {51, 1},
    {52, 0},
    {53, 1},
    {54, 0},
    {55, 1},
    {56, 0},
    {57, 1},
    {75, 1}};

std::map<int, std::string> funct2 = {
    {12, "0000"},
    {13, "0000"},
    {14, "0000"},
    {15, "0000"},
    {16, "0001"},
    {17, "0001"},
    {18, "0001"},
    {19, "0001"},
    {20, "0010"},
    {21, "0010"},
    {22, "0010"},
    {23, "0010"},
    {24, "0011"},
    {25, "0011"},
    {26, "0011"},
    {27, "0011"},
    {28, "0100"},
    {29, "0100"},
    {30, "0100"},
    {31, "0101"},
    {32, "0101"},
    {33, "0101"},
    {34, "0110"},
    {35, "0110"},
    {36, "0110"},
    {37, "0110"},
    {38, "0111"},
    {58, "0000"},
    {59, "0010"},
    {60, "0010"},
    {61, "0010"},
    {62, "0010"},
    {63, "1010"},
    {64, "1010"},
    {65, "1010"},
    {66, "1010"},
    {67, "0100"},
    {68, "1101"},
    {69, "1110"},
    {70, "1110"},
    {71, "1110"},
};

std::map<int, std::string> funct1 = {
    {4, "00"},
    {5, "01"},
    {6, "10"},
    {7, "11"},
    {12, "00"},
    {13, "01"},
    {14, "10"},
    {15, "11"},
    {16, "00"},
    {17, "01"},
    {18, "10"},
    {19, "11"},
    {20, "00"},
    {21, "01"},
    {22, "10"},
    {23, "11"},
    {24, "00"},
    {25, "01"},
    {26, "10"},
    {27, "11"},
    {28, "01"},
    {29, "10"},
    {30, "11"},
    {31, "01"},
    {32, "10"},
    {33, "11"},
    {34, "00"},
    {35, "01"},
    {36, "10"},
    {37, "11"},
    {40, "00"},
    {41, "01"},
    {42, "10"},
    {43, "11"},
    {44, "00"},
    {45, "01"},
    {46, "10"},
    {47, "11"},
    {59, "00"},
    {60, "01"},
    {61, "10"},
    {62, "11"},
    {63, "00"},
    {64, "01"},
    {65, "10"},
    {66, "11"},
    {67, "00"},
    {68, "00"},
    {69, "00"},
    {70, "01"},
    {71, "10"}};

std::string Instruction::getOpCode(int id)
{
    return opcode[id - 1];
}

std::string Instruction::getOpBit(int id)
{
    return std::to_string(opMap[id]);
}

std::string Instruction::getFunct2(int id)
{
    return funct2[id];
}

std::string Instruction::getFunct1(int id)
{
    return funct1[id];
}

std::string printRegister(int reg)
{
    switch (reg)
    {
    case HeapArrayRegister:
        return "$H0";
    case AcumulatorRegister:
        return "$A0";
    case TemporaryRegister:
        return "$T1";
    case FramePointer:
        return "$FP";
    case GlobalPointer:
        return "$GP";
    case BaseAddressRegister:
        return "$BA";
    case ReturnAddressRegister:
        return "$RA";
    case SwapRegister:
        return "$SR";
    }
    return "!UNKNOWN!";
}
