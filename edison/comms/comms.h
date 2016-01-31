#include <time.h> 
#include <gsl/gsl_matrix.h>
#include <limits.h>
#include <stdint.h>

typedef uint8_t encoded_word;

//#pragma pack(push) //pragma is needed to remove padding (extra spacing)
//#pragma pack(1)
struct comms_Packet {
  struct tm time;
  int data[3];
};
//#pragma pack(pop)

char comms_address[20];
int comms_codelen; // 6 <= codelen <= 2 in bits

int comms_sendMessage(char *buffer, int len);
char *comms_receiveMessage(int len);
//char *comms_EncodeMessage(struct comms_Packet pck);
//struct comms_Packet comms_DecodeMessage(char *buffer);

int comms_hammingdistance(encoded_word a, encoded_word b);
encoded_word *comms_hadamard(int comms_codelen);
gsl_matrix *comms_KPro(gsl_matrix *a, gsl_matrix *b);
int comms_printbin (encoded_word dec);
