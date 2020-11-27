#include "library.h"
#include <fstream>

void LibraryIncluder::setFileSource(std::string sourceFile)
{
    sourceFileName = sourceFile;
}

void LibraryIncluder::buildLibrary()
{
    std::string lib [] = {
        "int input(void) {}",
        "void output(int number) {}"};
    
    library.assign(std::begin(lib), std::end(lib));
}

FILE *LibraryIncluder::getFinalFile()
{
    buildLibrary();
    std::ofstream outputFile("temporary.o");
    std::ifstream inputFile(sourceFileName);
    std::string line;

    for(auto fun : library)
        outputFile << fun << std::endl;

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

void OSLibraryIncluder::buildLibrary()
{
    std::string lib[] = {
        "int input(void) {}",
        "void output(int number) {}",
        "int readFromMemory(int address){}",
        "int extractFirstHW(int number){}",
        "int extractSecondHW(int number){}",
        "void assignPointer(int vector[], int address){}"};

    library.assign(std::begin(lib), std::end(lib));
}
