#include "linker.h"
#include "bytes.h"

void Linker::link()
{
    printEveryLabelLink();

    insertIndexInsideEveryInstruction();

    for (auto origin : labelOriginMap)
    {
        std::string label = origin.first;
        for (BranchLabel *label_dest : origin.second)
        {
            int destinationAddress =
                labelDestMap[label]->relativeAddress -
                label_dest->branch->relativeAddress;
            Bytes number = Bytes(destinationAddress);
            label_dest->firstByte->debugAddress = labelDestMap[label]->relativeAddress;
            label_dest->firstByte->immediate = number.getNthByte(2);
            label_dest->secondByte->immediate = number.getNthByte(3);

            // Prints
            std::cout << "LINKER PRINTS\n";
            std::cout << label << " -> " << label_dest->to_string() << "\n";
            std::cout << "destinationAddress: " << destinationAddress << "\n";
            std::cout << "first Byte : " << label_dest->firstByte->to_string() << "\n";
            std::cout << "second Byte: " << label_dest->secondByte->to_string() << "\n";
            std::cout << "branch position: " << label_dest->branch->relativeAddress + offset << "\n";
            std::cout << "absolute position: " << labelDestMap[label]->relativeAddress + offset << "\n";
        }
    }
}

void Linker::printEveryLabelLink()
{
    for (auto item : labelDestMap)
    {
        std::string label = item.first;
        Instruction *label_dest = item.second;
        std::cout << label << " -> " << label_dest->name << "\n";
    }
}

void Linker::insertIndexInsideEveryInstruction()
{
    int i = 0;
    for (Instruction *inst : code)
        inst->relativeAddress = i++;
}

Linker *Linker::withCode(std::vector<Instruction *> _code)
{
    code = _code;
    return this;
}
Linker *Linker::withDestMap(std::map<std::string, Instruction *> map)
{
    labelDestMap = map;
    return this;
}
Linker *Linker::withOriginMap(std::map<std::string, std::vector<BranchLabel *>> map)
{
    labelOriginMap = map;
    return this;
}
Linker *Linker::withOffset(int value)
{
    offset = value;
    return this;
}
