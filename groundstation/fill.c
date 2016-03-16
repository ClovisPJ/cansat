#include <stdio.h>
#include <stdlib.h>

int main() {
  FILE *f;

  //for(int j = 0; j <= 100; j++) {
  while (1) {
    f = fopen("data.csv", "a");

    for (int i = 0; i < 25; i++) {
      fputc((rand()%10)+'0', f);
      fputc(',', f);
    }
    fputc((rand()%10)+'0', f);
    fputc('\n', f);

    fclose(f);
    usleep(1000000);
  }
}
