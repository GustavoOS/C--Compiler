#include "codegenerator.h"

TypeAInstruction::TypeAInstruction(int identity, std::string instructionName,
                                   int instructionImmediate, int RegisterM, int RegisterD)
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

class TypeCInstruction : public Instruction
{
    std::string to_string()
    {
        return "(" + std::to_string(id) + ") " +
               name + " " + std::to_string(immediate) + " " +
               std::to_string(regm) + " " + std::to_string(regd) + "\n";
    }
};

class TypeDInstruction : public Instruction
{
    std::string to_string()
    {
        return "(" + std::to_string(id) + ") " +
               name + " " + std::to_string(regd) + " " +
               std::to_string(immediate) + "\n";
    }
};

class TypeEInstruction : public Instruction
{
    std::string to_string()
    {
        return "(" + std::to_string(id) + ") " +
               name + " " + std::to_string(regm) + " " +
               std::to_string(regd) + "\n";
    }
};

class TypeFInstruction : public Instruction
{
    std::string to_string()
    {
        return "(" + std::to_string(id) + ") " +
               name + " " + std::to_string(condition) + " " +
               std::to_string(regd) + "\n";
    }
};

class TypeGInstruction : public Instruction
{
    std::string to_string()
    {
        return "(" + std::to_string(id) + ") " +
               name + " " + std::to_string(condition) + " " +
               std::to_string(offset) + " :" + label + "\n";
    }
};

class LabelDefinition : public Instruction
{
    std::string to_string()
    {
        return label + ":\n";
    }
};