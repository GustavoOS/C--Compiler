/* Um programa para calcular o mdc
   segundo o algoritmo de Euclides. */

int gcd(int u, int v)
{
    output(u);
    output(v);
    if (v == 0)
        return u;
    else
        return gcd(v, u - u / v * v);
}

void main(void)
{
    int i;
    i = gcd(16, 24);
    output(i);
}
