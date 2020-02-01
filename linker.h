#ifndef _LINKER_
#define _LINKER_
#include <iostream>
#include "codegenerator.h"

class Linker
{
public:
    void link();

    Linker *withCode(std::vector<Instruction *> _code);
    Linker *withDestMap(std::map<std::string, Instruction *> map);
    Linker *withOriginMap(std::map<std::string, std::vector<BranchLabel *>> map);
    Linker *withOffset(int value);

private:
    int offset;
    std::vector<Instruction *> code;
    std::map<std::string, Instruction *> labelDestMap;
    std::map<std::string, std::vector<BranchLabel *>> labelOriginMap;

    void printEveryLabelLink();
    void insertIndexInsideEveryInstruction();
};

#endif
