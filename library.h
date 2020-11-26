#ifndef __LIBRARY_DEFINITION__
#define __LIBRARY_DEFINITION__
#include <iostream>
#include <list>

class LibraryIncluder
{
public:
    void setFileSource(std::string sourceFileName);
    FILE *getFinalFile();
    std::list<std::string> library;

protected:
    FILE *source;
    std::string sourceFileName;

    virtual void buildLibrary();
};

class OSLibraryIncluder : public LibraryIncluder
{

protected:
    void buildLibrary();
};

#endif
