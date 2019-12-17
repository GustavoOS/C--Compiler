int main(void)
{
    int number;
    number = readFromMemory(16384);
    output(number);
    writeIntoMemory(2048, number);
    number = readFromMemory(2048);
    output(number);
    return 0;
}
