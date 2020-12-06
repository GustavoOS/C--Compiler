int main(void)
{
    int i;

    while(i < 11)
    {
        output(i);
        output(i | 10);
        i = i + 1;
    }
    return 0;
}
