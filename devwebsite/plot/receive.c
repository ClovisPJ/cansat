#include "comms.h"
#include <math.h>
#include <string.h>

int main() {

  comms_codelen = 4;
  // to change this, also change typedef for code
  strcpy(comms_address, "/dev/ttyACM0");
  struct comms_Packet pck;

  while(1) {
    pck = comms_UnpackMessage(comms_receiveMessage(sizeof(struct comms_Packet)));
    FILE *f;
    f = fopen("values.txt", "w");
    fputc(pck.data[0], f);
    fclose(f);
    usleep(1000000);
  }

  return 0;
}
