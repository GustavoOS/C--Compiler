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

enum OperandType{
    referenceOperand,
    numberOperand,
    variableOperand
};

class Operand {
    public:
    OperandType type;
    int tripleIndex;
    int number;
    std::string name;
    std::string scope;
    void setAsVariable(std::string name, std::string scope);
    void setAsNumber(int no);
    void setAsReference(int index);
};

class Triple
{

  public:
    Triple(Operation _operation,  Operand *_firstOPerand,
            Operand *_secondOPerand);

  private:
    Operation operation;
    Operand *firstOperand, *secondOperand;
};

class TripleList
{
    std::vector<Triple *> list;

  public:
    void addTriple(Triple *tripleItem);
};

#endif