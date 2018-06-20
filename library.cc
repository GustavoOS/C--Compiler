#include "library.h"
#include <fstream>

LibraryIncluder::LibraryIncluder(std::string sourceFile)
{
    sourceFileName = sourceFile;
}

void LibraryIncluder::buildLibrary()
{
    libraryString += "int input(void) {}\n";
    libSize++;
    libraryString += "void output(int number) {}\n";
    libSize++;
    libraryString += "void outputLED(int number) {}\n";
    libSize++;
}

FILE *LibraryIncluder::getFinalFile()
{
    buildLibrary();
    std::ofstream outputFile("temporary.o");
    std::ifstream inputFile(sourceFileName);
    std::string line;
    // std::stringstream buffer;
    // buffer << libraryString << inputFile.rdbuf();
    outputFile << libraryString;
    if (inputFile.is_open())
        while (getline(inputFile, line))
        {
           outputFile << line << "\n";
        };
    outputFile << "\n";
    inputFile.close();
    outputFile.close();
    // std::cout << buffer.str() << std::endl;

    // size_t bufferSize = 0;
    // char *f_buffer = NULL;
    // source = open_memstream(&f_buffer, &bufferSize);
    // snprintf(f_buffer, bufferSize, buffer.str().c_str());

    source = fopen("temporary.o", "r");
    // source = fopen(sourceFileName.c_str(), "r") ;
    return source;
}