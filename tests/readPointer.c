int main(void)
{
    int test[0];

    assignPointer(test, 255);
    if (readPointer(test) == 255)
        output(1253); // 4e5
    else
        output(51914); // caca
    
    return 0;
}
