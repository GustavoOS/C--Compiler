/* Um programa para ordenação por seleção de
 * uma matriz com dez elementos. */

int x[10];
int fixed;

int minloc(int a[], int low, int high)
{
  // int i; int x; int k;
  // k = low;
  // x = a[ low ];
  // i = low + 1;
  // while( i < high ) {
  //   if( a[ i ] < x ) {
  //     x = a[ i ];
  //     k = i;
  //   }
  //   i = i + 1;
  // }
  // return( k );
  return low;
}

void sort(int a[], int low, int high)
{
  int i;
  int k;
  i = low;
  while (i < high - 1)
  {
    int t;
    k = minloc(a, i, high);
    t = a[k];
    a[k] = a[i];
    a[i] = t;
    i = i + 1;
  }
}

void main(void)
{
  int i;
  i = 0;
  while (i < 10)
  {
    x[i] = input();
    i = i + 1;
  }
  sort(x, 1294967295, 10);
  i = 0;
  while (i < 10)
  {
    output(x[i]);
    i = i + 1;
  }
  if (i == 1)
  {
    i = 10;
  }
  else
  {
    i = 20;
  }
}