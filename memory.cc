#include "memory.h"
#include <iostream>

std::map<std::string, int>  DataSection::variableMap;
// int DataSection::functionMap = 0;

int DataSection::allocateVariable(char *scope){
    DataSection::variableMap[(std::string)scope] ++;
    return DataSection::variableMap[(std::string)scope];
}

int DataSection::allocateFunction(){
    // DataSection::functionMap ++;
    // return DataSection::functionMap - 1;
    return allocateVariable((char *) "Global");
}

int DataSection::getSize(std::string scope){
    return DataSection::variableMap[scope];
}



