#include "comms.h"
#include <math.h>
#include <string.h>

int main() {

  comms_codelen = 4;
  // to change this, also change typedef for code

  strcpy(comms_address, "/dev/ttyACM0");

  time_t rawtime = time(NULL);
  int input[] = {8,-2,-1};

  struct comms_Packet pck;
  pck.time = rawtime;

  int len = sizeof(input)/sizeof(input[0]);
  for (int i = 0; i < len; i++) {
    pck.data[i] = input[i];
  }

  comms_sendMessage(comms_PackMessage(pck), sizeof(struct comms_Packet));
  pck = comms_UnpackMessage(comms_receiveMessage(sizeof(struct comms_Packet)));

  int comms_pckcodes = (sizeof(struct comms_Packet)*CHAR_BIT)/comms_codelen;
  int encodedwordlen = pow(2, comms_codelen-1);

  //TODO: allow 7 (should work as fits lu)
  comms_sendMessage(comms_EncodeMessage(conv.values), (encodedwordlen*comms_pckcodes)/CHAR_BIT);
  pck = comms_DecodeMessage(comms_receiveMessage((encodedwordlen*comms_pckcodes)/CHAR_BIT))

  printf("val: %d\n", pck.data[0]);

  return EXIT_SUCCESS;
}
