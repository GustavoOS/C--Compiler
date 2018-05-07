#include "intermediate.h"
#include <vector>

Triple::Triple(Operation _operation, Operand _firstOPerand, Operand _secondOPerand)
{
    operation = _operation;
    firstOperand = _firstOPerand;
    secondOperand = _secondOPerand;
};

void TripleList::addTriple()
{
    Triple myTriple;
    list.push_back(myTriple);
}