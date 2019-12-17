#ifndef _MIF_GENERATOR_
#define _MIF_GENERATOR_
#include <iostream>
#include <fstream>

class MifGenerator
{
public:
    MifGenerator();
    void open(std::string, bool);
    void printMultipleEmptyPosition(int start, int repeats);
    void printInstruction(int, std::string, std::string);
    void printDebugMsg(std::string);
    void jumpLine();
    void printFooter();
private:
    std::string header;
    std::ofstream file;
    void close();
    void printLine(std::string);
    void printEmptyMemoryPosition(int);
    bool isBios;
};

#endif
