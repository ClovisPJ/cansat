#include "edison/rfm69.h"
#include "../exttm.h"

int main() {
  rfm69_settings();
  char *str = "Hello World!";
  while (1) {
    rfm69_send(str, 12);
    usleep(1000000);
  }
}
