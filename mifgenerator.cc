#include "mifgenerator.h"
#include <iostream>
#include <fstream>

MifGenerator::MifGenerator()
{
    header = "-- begin_signature\n"
             "-- Memory\n"
             "-- end_signature\n";
    greatestLine = -1;
}

void MifGenerator::open(std::string outputFile, bool isBios, bool isCompressed)
{
    this->isCompressed = isBios || isCompressed;
    std::string width = isBios ? "16;\n" : "32;\n";
    std::string depth = isBios ? "512;\n" : "16384;\n";
    dontcare = isBios ? "XXXXXXXXXXXXXXXX"
                      : "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    header += "WIDTH=" + width +
              "DEPTH=" + depth +
              "\n"
              "ADDRESS_RADIX=UNS;\n"
              "DATA_RADIX=BIN;\n"
              "\n"
              "CONTENT BEGIN\n";
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
    updateGreatestLine(position);
    printLine("    " + std::to_string(position) +
              " : " +
              dontcare + ";\n");
}

void MifGenerator::printInstruction(int position, std::string binary, std::string assembly)
{
    updateGreatestLine(position);
    std::string firstHW = isCompressed ? "" : "0000000000000000";
    printLine("    " + std::to_string(position) +
              " : " + firstHW +
              binary + "; -- " + assembly);
}

void MifGenerator::printMultipleEmptyPosition(int repeats)
{
    int start = greatestLine + 1;
    for (int i = start; i < repeats; i++)
        printEmptyMemoryPosition(i);
}

void MifGenerator::printDebugMsg(std::string msg)
{
    printLine(" (" + msg + ")");
}

void MifGenerator::updateGreatestLine(int line)
{
    if (line > greatestLine)
        greatestLine = line;
}
