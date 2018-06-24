#ifndef _MEMORY_MANAGEMENT_
#define _MEMORY_MANAGEMENT_

#include <iostream>
#include <map>

class DataSection
{
  public:
    int allocateVariable(std::string scope);
    int allocateFunction();
    int getSize(std::string);
    private:
    static std::map<std::string, int> variableMap;
    // static int functionMap;
};

#endif