#ifndef __LIBRARY_DEFINITION__
#define __LIBRARY_DEFINITION__
#include <iostream>

class LibraryIncluder
{
  public:
    LibraryIncluder(FILE *);
    FILE *getFinalFile();

    int libSize = 0;

  private:
    FILE *originalSource;
    FILE *source;
    std::string libraryString;

    void buildLibrary();
};

#endif