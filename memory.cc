#include "memory.h"
#include <iostream>

std::map<std::string, int> DataSection::variableMap;
// int DataSection::functionMap = 0;

int DataSection::allocateVariable(std::string scope)
{
    DataSection::variableMap[scope]++;
    return DataSection::variableMap[scope];
}

int DataSection::allocateFunction()
{
    return 0;
}

int DataSection::getSize(std::string scope)
{
    return DataSection::variableMap[scope];
}
