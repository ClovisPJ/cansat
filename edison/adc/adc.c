#include <stdio.h>
#include "ads1115.h"

int main() {
  ads1115_init();
  double val;
  while (1) {
    val = ads1115_read();
    printf("value is: %fV\n", val);
    usleep(100000);
  }
  ads1115_stop();

}
