#include <stdio.h>
#include <stdlib.h>

int main() {
  FILE *f;

  //for(int j = 0; j <= 100; j++) {
  while (1) {
    f = fopen("tmp", "w");

    fputc((rand()%10)+'0', f);
    fputc((rand()%10)+'0', f);
    fputc((rand()%10)+'0', f);

    fputc('\n', f);

    fputc((rand()%10)+'0', f);
    fputc((rand()%10)+'0', f);
    fputc((rand()%10)+'0', f);

    fclose(f);
    usleep(1000000);
  }
}
