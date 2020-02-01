#include <assert.h>
#include "binarygenerator.h"

#define MEMORY_SIZE 16384
#define BIOS_SIZE 512

BinaryGenerator *BinaryGeneratorFactory::generate(bool isCompressed, bool isBios, bool isOS)
{
    if (isCompressed)
        return (BinaryGenerator *)new CompressedBinaryGenerator();
    if (isBios)
        return (BinaryGenerator *)new BIOSBinaryGenerator();
    if (isOS)
        return (BinaryGenerator *)new OSBinaryGenerator();
    return (BinaryGenerator *)new UncompressedBinaryGenerator();
}

void BinaryGenerator::setFile(std::string file)
{
    fileName = file;
}

void BinaryGenerator::setOffset(int value)
{
    offset = value;
}

void UncompressedBinaryGenerator::run(std::vector<Instruction *> code)
{
    printf("\n\n +++++ Code generator! +++++ \n\n");
    mif.open(fileName, false, false);
    mif.printMultipleEmptyPosition(offset);
    for (Instruction *inst : code)
    {
        std::string bin = inst->to_binary();
        assert(bin.size() == 16);

        if (!inst->debugname.empty())
            printf("%s\n", inst->debugname.c_str());

        printf("% 3d: %-22s => %s\n",
               inst->relativeAddress,
               inst->to_string().c_str(),
               bin.c_str());

        mif.printInstruction(inst->relativeAddress + offset,
                             bin,
                             inst->to_string());

        if (!inst->debugname.empty())
            mif.printDebugMsg(inst->debugname);

        mif.jumpLine();
    }

    mif.printMultipleEmptyPosition(MEMORY_SIZE);
    mif.printFooter();

    std::cout << "\n\n Output saved on %s \n\n"
              << fileName.c_str();
}

void CompressedBinaryGenerator::run(std::vector<Instruction *> code)
{
    std::cout << "Compressed File\n";
    int headerSize = 1;

    mif.open(fileName, false, true);

    mif.printSize(code.size(), offset);

    for (int i = 0; i < (int)code.size(); i += 2)
    {
        bool hasNext = (i + 1) < (int)code.size();
        std::string leftInstr = code[i]->to_binary();
        std::string rightInstr = hasNext
                                     ? code[i + 1]->to_binary()
                                     : "0000000000000000";
        std::string debugText = code[i]->to_string();
        if (hasNext)
            debugText += " | " + code[i + 1]->to_string();

        mif.printInstruction(offset + headerSize + ((int)i / 2),
                             leftInstr + rightInstr,
                             debugText);

        if (!code[i]->debugname.empty())
            mif.printDebugMsg(code[i]->debugname);
        if (hasNext && (!code[i + 1]->debugname.empty()))
            mif.printDebugMsg(code[i + 1]->debugname);

        mif.jumpLine();
    }

    mif.printMultipleEmptyPosition(MEMORY_SIZE);

    mif.printFooter();
    printf("\n\n Output saved on %s \n\n", fileName.c_str());
}

void BIOSBinaryGenerator::run(std::vector<Instruction *> code)
{
    printf("\n\n +++++ Code generator! +++++ \n\n");
    mif.open(fileName, true, false);
    mif.printMultipleEmptyPosition(offset);

    for (Instruction *inst : code)
    {
        std::string bin = inst->to_binary();
        assert(bin.size() == 16);

        if (!inst->debugname.empty())
            printf("%s\n", inst->debugname.c_str());

        printf("% 3d: %-22s => %s\n",
               inst->relativeAddress,
               inst->to_string().c_str(),
               bin.c_str());

        mif.printInstruction(inst->relativeAddress + offset,
                             bin,
                             inst->to_string());

        if (!inst->debugname.empty())
            mif.printDebugMsg(inst->debugname);

        mif.jumpLine();
    }

    mif.printMultipleEmptyPosition(BIOS_SIZE);
    mif.printFooter();
    printf("\n\n Output saved on %s \n\n", fileName.c_str());
}

void OSBinaryGenerator::run(std::vector<Instruction *> code)
{

    printf("\n\n +++++ Code generator! +++++ \n\n");
    mif.open(fileName, false, false);

    mif.printMultipleEmptyPosition(offset);
    mif.printUncompressedSize(code.size(), offset);
    offset++;

    for (Instruction *inst : code)
    {
        std::string bin = inst->to_binary();
        assert(bin.size() == 16);

        if (!inst->debugname.empty())
            printf("%s\n", inst->debugname.c_str());

        printf("% 3d: %-22s => %s\n",
               inst->relativeAddress,
               inst->to_string().c_str(),
               bin.c_str());

        mif.printInstruction(inst->relativeAddress + offset,
                             bin,
                             inst->to_string());

        if (!inst->debugname.empty())
            mif.printDebugMsg(inst->debugname);

        mif.jumpLine();
    }

    mif.printMultipleEmptyPosition(MEMORY_SIZE);
    mif.printFooter();
    printf("\n\n Output saved on %s \n\n", fileName.c_str());
}