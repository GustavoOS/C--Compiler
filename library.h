#ifndef __LIBRARY_DEFINITION__
#define __LIBRARY_DEFINITION__
#include <iostream>

class LibraryIncluder
{
  public:
    LibraryIncluder(std::string sourceFileName);
    FILE *getFinalFile();

    int libSize = 0;

  private:

    FILE *source;
    std::string libraryString;
    std::string sourceFileName;

    void buildLibrary();
};

#endif