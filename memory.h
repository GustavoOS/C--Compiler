#ifndef _MEMORY_MANAGEMENT_
#define _MEMORY_MANAGEMENT_

#include <iostream>
#include <map>

class DataSection
{
  public:
    int allocateVariable(char * scope);
    int allocateFunction();
  private:  
    static std::map<std::string, int> variableMap;
    static int functionMap;
};

#endif