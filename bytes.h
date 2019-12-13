#ifndef _BYTES_
#define _BYTES_

class Bytes
{
private:
    int bytes[4];
    int convertNthByteToInt(int, int);

public:
    Bytes(int);
    int getNthByte(int); //Zero based
};

#endif
