#ifndef _BYTES_
#define _BYTES_

#include<iostream>

class Bytes
{
private:
    int bytes[4];
    int convertNthByteToInt(int, int);

public:
    Bytes(int);
    int getNthByte(int); //Zero based
};

std::string getVal5Bits(int val);
std::string getVal3Bits(int val);
std::string getVal4Bits(int val);
std::string getVal8Bits(int val);
std::string getVal8BitsSignal(int val);

#endif
