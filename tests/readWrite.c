int main(void)
{
    int address;
    int data;

    address = 0;
    data = readFromMemory(address);
    output(data);
    writeIntoMemory(address, data + 1);
    data = readFromMemory(address);
    output(data);

    return 0;
}
