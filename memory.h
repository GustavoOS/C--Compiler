#ifndef _MEMORY_MANAGEMENT_
#define _MEMORY_MANAGEMENT_


#include <map>

class DataSection
{
  public:
    int allocateVariable(char * scope, int size);
    int allocateFunction();
  private:  
    static std::map<std::string, int> variableMap;
    static int functionMap;
};

#endif