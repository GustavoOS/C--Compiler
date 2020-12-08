void assert(int actual, int expected)
{
    if (actual == expected)
    {
        output(1253); // 4e5
    }
    else
    {
        output(51914); // caca
    }
}

int main(void)
{
    assert(237 & 60, 44); // Mask
    assert(5 | 3, 7);
    assert(5 ^ 3, 6);
    assert(11 % 10, 1);
    return 0;
}
