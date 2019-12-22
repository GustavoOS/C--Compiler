#include "bytes.h"
#include <iostream>
#include <bitset>

Bytes::Bytes(int number)
{
    fullNumber = std::bitset<32>(number).to_string();
}

std::string Bytes::to_string()
{
    return fullNumber;
}

int Bytes::getNthByte(int nthByte)
{
    std::string byteNumber = fullNumber.substr(8 * nthByte, 8);
    std::bitset<8> partialNumber(byteNumber);
    return (int)partialNumber.to_ulong();
}

std::string getVal5Bits(int val)
{
    std::bitset<5> bs(val);
    return bs.to_string();
}

std::string getVal3Bits(int val)
{
    std::bitset<3> bs(val);
    return bs.to_string();
}

std::string getVal4Bits(int val)
{
    std::bitset<4> bs(val);
    return bs.to_string();
}

std::string getVal8Bits(int val)
{
    std::bitset<8> bs(val);
    return bs.to_string();
}

std::string getVal8BitsSignal(int val)
{
    std::bitset<8> bs(val);
    return bs.to_string();
}
