#include "bytes.h"
#include <iostream>
#include <bitset>

Bytes::Bytes(int number)
{
    for (int i = 0; i < 4; i++)
        bytes[i] = convertNthByteToInt(number, i);
}

int Bytes::getNthByte(int nthByte)
{
    return bytes[nthByte];
}

int Bytes::convertNthByteToInt(int number, int nthByte)
{
    std::string fullNumber = std::bitset<32>(number).to_string();
    std::string byteNumber = fullNumber.substr(8 * nthByte, 8);
    std::bitset<8> partialNumber(byteNumber);
    int n = (int)partialNumber.to_ulong();
    // std::cout << "This is Bytes, returning " << n << "from the number "
    //   << number << ". That should be the " << nthByte << " position byte\n";
    return n;
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
    // TODO : Add support to signal
    return bs.to_string();
}