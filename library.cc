#include "library.h"

LibraryIncluder::LibraryIncluder(FILE *file)
{
    originalSource = file;
}

void LibraryIncluder::buildLibrary()
{
    libraryString += "int input() {}\n"; libSize++;
    libraryString += "void output(int number) {}\n"; libSize++;
    libraryString += "void outputLED(int number) {}\n"; libSize++;
}

FILE *LibraryIncluder::getFinalFile()
{
    source = fopen("temporary.o", "w+");
    buildLibrary();
    std::cout << fprintf(source, (const char *)libraryString.c_str()) << "\n";
    char c = fgetc(originalSource);
    while (c != EOF)
    {
        // std::cout << c;
        fputc(c, source);
        c = fgetc(originalSource);
    }
    fclose(originalSource);
    return source;
}