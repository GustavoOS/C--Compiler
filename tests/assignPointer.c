int main(void)
{
    int address;
    int data [0];

    address = 0;
    assignPointer(data, address);
    output(data[0]);
    data[0] = data[0] + 1;
    output(data[0]);

    return 0;
}
