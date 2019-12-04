#ifndef _MIF_GENERATOR_
#define _MIF_GENERATOR_
#include <iostream>
#include <fstream>

class MifGenerator
{
public:
    MifGenerator(std::string);
    void printEmptyMemoryPosition(int);
    void printInstruction(int, std::string, std::string);
    void printLine(std::string);
    void printHeader();
    void jumpLine();
    void printFooter();
private:
    std::string header;
    std::ofstream file;
    void close();
};

#endif
