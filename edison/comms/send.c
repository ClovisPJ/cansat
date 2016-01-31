#include "comms.h"
#include <string.h>


int main() {

  comms_codelen = 4;
  // to change this, also change typedef for code

  strcpy(comms_address, "/dev/ttyACM0");

  clock_t rawtime = time(NULL);
  int input[] = {1,-2,-1};

  struct comms_Packet pck;
  pck.time = *localtime(&rawtime);

  int len = sizeof(input)/sizeof(input[0]);
  for (int i = 0; i < len; i++) {
    pck.data[i] = input[i];
  }

  union {
    struct comms_Packet packet;
    unsigned char values[sizeof(struct comms_Packet)];
  } conv;
  conv.packet = pck;

  comms_sendMessage(conv.values, sizeof(struct comms_Packet));
//  strncpy(conv.values, comms_receiveMessage(sizeof(struct comms_Packet)), sizeof(struct comms_Packet));

 // printf("val 1: %d", conv.packet.data[0]);

/*  int comms_pckcodes = (sizeof(struct comms_Packet)*CHAR_BIT)/comms_codelen;
  int encodedwordlen = pow(2, comms_codelen-1);
  comms_sendMessage(comms_EncodeMessage(conv.values), (encodedwordlen*comms_pckcodes)/CHAR_BIT);
  conv.values = comms_DecodeMessage(comms_receiveMessage((encodedwordlen*comms_pckcodes)/CHAR_BIT));
*/
  //TODO: allow 7 (should work as fits lu)

  return EXIT_SUCCESS;
}
