#include "memory.h"
#include <iostream>

std::map<std::string, int>  DataSection::variableMap;
int DataSection::functionMap = 0;

int DataSection::allocateVariable(char *scope, int size){
    DataSection::variableMap[(std::string)scope] += size;
    return DataSection::variableMap[(std::string)scope] - size;
}

int DataSection::allocateFunction(){
    DataSection::functionMap ++;
    return DataSection::functionMap - 1;
}



