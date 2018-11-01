#include <stdio.h>
int main(void)
{
  int x, y;
  y = 0;
  x = 1/y;
  y = x;
  printf("x: %d, y: %d\n",x,y);
}
