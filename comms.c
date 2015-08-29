#include <stdio.h>

int matchcode();
int hammingdistance();

int main() {
  int x = 0b11;
  printf("this should be: %i\n",matchcode(x));
  return 0;
}

int matchcode(int a) {
  int hamsqa[2] = {0b11,0b10};
  int min = 2;
  int imin = 0;
  int i = 0;
  for (i < 2; i++;) {
    int hd = hammingdistance(a,hamsqa[i]);
    if (hd < min) {
      min = hd;
      imin = i;
    }
  }
  return imin;
}

int hammingdistance(int a, int b) {
  int z = a^b;
  int n = 0;
  while (z) {
    if (z&1){
      n++;
    }
    z >>= 1;
  }
  return n;
}
