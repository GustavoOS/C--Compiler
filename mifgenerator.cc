#include "mifgenerator.h"
#include <iostream>
#include <fstream>

MifGenerator::MifGenerator()
{
    header = "-- begin_signature\n"
             "-- Memory\n"
             "-- end_signature\n"
             "WIDTH=32;\n"
             "DEPTH=16384;\n"
             "\n"
             "ADDRESS_RADIX=UNS;\n"
             "DATA_RADIX=BIN;\n"
             "\n"
             "CONTENT BEGIN\n";
}

void MifGenerator::open(std::string outputFile, bool isBios)
{
    this->isBios = isBios;
    file.open(outputFile.c_str(), std::ofstream::out);
    file << header;
}

void MifGenerator::printLine(std::string line)
{
    file << line;
}

void MifGenerator::jumpLine()
{
    file << "\n";
}

void MifGenerator::printFooter()
{
    file << "END;\n"
         << std::endl;

    close();
}

void MifGenerator::close()
{
    file.close();
}

void MifGenerator::printEmptyMemoryPosition(int position)
{
    std::string firstHW = isBios ? "" : "XXXXXXXXXXXXXXXX";
    printLine("    " + std::to_string(position) +
              " : " +
              firstHW +
              "XXXXXXXXXXXXXXXX;\n");
}

void MifGenerator::printInstruction(int position, std::string binary, std::string assembly)
{
    std::string firstHW = isBios ? "" : "0000000000000000";
    printLine("    " + std::to_string(position) +
              " : " + firstHW +
              binary + "; -- " + assembly);
}

void MifGenerator::printMultipleEmptyPosition(int start, int repeats)
{
    for (int i = start; i < repeats; i++)
        printEmptyMemoryPosition(i);
}

void MifGenerator::printDebugMsg(std::string msg)
{
    printLine(" (" + msg + ")");
}
