#ifndef _INTERMEDIATE_CODE_
#define _INTERMEDIATE_CODE_

#include <vector>
#include <iostream>

enum Operation
{
    sum,
    multiply,
    subtract,
    divide,
    accessVector,
    functionCall /*TO DO*/
};

union Operand {
    int tripleIndex;
    int number;
    struct
    {
        std::string name;
        std::string scope;
    } variable;
};

class Triple
{

  public:
    Triple(Operation _operation, Operand _firstOPerand,
           Operand _secondOPerand);

  private:
    Operation operation;
    union Operand firstOperand, secondOperand;
};

class TripleList
{
    std::vector<Triple *> list;

  public:
    void addTriple(Triple *tripleItem);
};

#endif