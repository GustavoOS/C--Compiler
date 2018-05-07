#include "intermediate.h"
#include <vector>

void Operand::setAsVariable(std::string name, std::string scope)
{
    this->type = variableOperand;
    this->name = name;
    this->scope = scope;
}

void Operand::setAsNumber(int no)
{
    this->type = numberOperand;
    this->number = no;
}

void Operand::setAsReference(int index)
{
    this->type = referenceOperand;
    this->tripleIndex = index;
}

Triple::Triple(Operation _operation, Operand *_firstOPerand, Operand *_secondOPerand)
{
    operation = _operation;
    firstOperand = _firstOPerand;
    secondOperand = _secondOPerand;
};

void TripleList::addTriple(Triple *addedTriple)
{
    list.push_back(addedTriple);
}