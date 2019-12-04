#include "mifgenerator.h"
#include <iostream>
#include <fstream>

MifGenerator::MifGenerator(std::string outputFile)
{

    file.open(outputFile.c_str(), std::ofstream::out);

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

void MifGenerator::printHeader()
{
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
    printLine("    " + std::to_string(position) +
              " : XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX;\n");
}

void MifGenerator::printInstruction(int position, std::string binary, std::string assembly)
{
    printLine("    " + std::to_string(position) +
                  " : 0000000000000000" + binary + "; -- " + assembly);
}
