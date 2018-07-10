/* Um programa para calcular o mdc
   segundo o algoritmo de Euclides. */

int x;

// int gcd(int u, int v)
// {
//     output(51966); //CAFE
//     output(u);
//     output(v);
//     // if (v == 0)
//     //     return u;
//     // else
//     //     return gcd(v, u - u / v * v);
//     /* u-u/v*v == u mod v */
//     return 4207594238; //FACACAFE
// }

// int func(void)
// {
//     y = 64202; //FACA
//     return y * 65536 + func(); //FACACAFE
// }

void main(void)
{
    x = 1;
    // x = 43962; //ABBA
    // y = 47802; // BABA
    // output(11);
    // x = input();
    // output(x);
    // output(13);
    // y = input();
    // output(y);
    // output(gcd(43690, 48059));
    // output(gcd(x, y));
    if( x == 1 ){
        output(43962); //ABBA    
    }else{
        output(47802); //ABBA    
    }
}
