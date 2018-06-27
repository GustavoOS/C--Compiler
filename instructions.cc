#include "codegenerator.h"

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

BranchLabel::BranchLabel(std::string gotolabel, TypeDInstruction *lByte, TypeDInstruction *rByte)
{
    tolabel = gotolabel;
    leftByte = lByte;
    rightByte = rByte;
}

std::string BranchLabel::to_string()
{
    return "Branch to " +  tolabel + "\n";
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
           std::to_string(regm) + " " + std::to_string(regd) + "\n";
};

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
           name + " " + std::to_string(regm) + " " +
           std::to_string(regn) + " " +
           std::to_string(regd) + "\n";
}

TypeCInstruction::TypeCInstruction(
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

std::string TypeCInstruction::to_string()
{
    return "(" + std::to_string(id) + ") " +
           name + " " + std::to_string(immediate) + " " +
           std::to_string(regm) + " " + std::to_string(regd) + "\n";
}

std::string TypeDInstruction::to_string()
{
    return "(" + std::to_string(id) + ") " +
           name + " " + std::to_string(regd) + " " +
           std::to_string(immediate) + "\n";
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
           name + " " + std::to_string(regm) + " " +
           std::to_string(regd) + "\n";
}

std::string TypeFInstruction::to_string()
{
    return "(" + std::to_string(id) + ") " +
           name + " " + std::to_string(condition) + " " +
           std::to_string(regd) + "\n";
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

TypeGInstruction::TypeGInstruction(
    int identity,
    std::string instructionName,
    int conditionCode,
    int offsetSize,
    std::string labelTo)
{
    id = identity;
    name = instructionName;
    condition = conditionCode;
    offset = offsetSize;
    label = labelTo;
}

std::string TypeGInstruction::to_string()
{
    return "(" + std::to_string(id) + ") " +
           name + " " + std::to_string(condition) + " " +
           std::to_string(offset) + " :" + label + "\n";
}