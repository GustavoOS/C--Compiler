#ifndef _BINARY_GENERATOR_
#define _BINARY_GENERATOR_

#include <iostream>
#include <vector>
#include "instructions.h"
#include "mifgenerator.h"

class BinaryGenerator
{
public:
    virtual void run(std::vector<Instruction *>) = 0;
    void setOffset(int);
    void setFile(std::string);

protected:
    std::string fileName;
    int offset;
    MifGenerator mif;
};

class BinaryGeneratorFactory
{
public:
    static BinaryGenerator *generate(bool isCompressed, bool isBios, bool isOS);
};

class UncompressedBinaryGenerator : public BinaryGenerator
{
public:
    void run(std::vector<Instruction *>);
};

class OSBinaryGenerator : public BinaryGenerator
{
public:
    void run(std::vector<Instruction *>);
};

class CompressedBinaryGenerator : public BinaryGenerator
{
public:
    void run(std::vector<Instruction *>);
};

class BIOSBinaryGenerator : public BinaryGenerator
{
public:
    void run(std::vector<Instruction *>);
};

#endif
