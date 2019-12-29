#ifndef _MIF_GENERATOR_
#define _MIF_GENERATOR_
#include <iostream>
#include <fstream>

class MifGenerator
{
public:
    MifGenerator();
    void open(std::string, bool, bool);
    void printMultipleEmptyPosition(int repeats);
    void printInstruction(int, std::string, std::string);
    void printDebugMsg(std::string);
    void printSize(int codeSize, int place);
    void printOSSize(int codeSize, int place);
    void jumpLine();
    void printFooter();
private:
    std::string header, dontcare;
    std::ofstream file;
    bool isCompressed;
    int greatestLine;
    void close();
    void printLine(std::string);
    void printEmptyMemoryPosition(int);
    void updateGreatestLine(int);
};

#endif
