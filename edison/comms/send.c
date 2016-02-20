#include <stdio.h>

#include "edison/rfm69.h"

int main() {
  rfm69_settings();
  char str[100];
  for (int i = 0; i < 100; i++) {
    str[i] = i+30;
  }
  while (1) {
    rfm69_send(str, 100);
    usleep(1000000);
  }
  /*char *str;
  str = rfm69_receive(100);
  str[100] = '\0';
  puts(str);*/
}
