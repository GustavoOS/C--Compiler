#include "memory.h"
#include <iostream>

std::map<std::string, int>  DataSection::variableMap;
int DataSection::functionMap = 0;

int DataSection::allocateVariable(char *scope){
    DataSection::variableMap[(std::string)scope] ++;
    return DataSection::variableMap[(std::string)scope] - 1;
}

int DataSection::allocateFunction(){
    DataSection::functionMap ++;
    return DataSection::functionMap - 1;
}



