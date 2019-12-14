int main(void)
{
    int address;
    int data;
    int one;

    one = 1;
    address = 0;
    data = readFromMemory(address);
    output(data);
    writeIntoMemory(address, data + one);
    data = readFromMemory(address);
    output(data);

    return 0;
}
