#include <time.h> 
#include <gsl/gsl_matrix.h>
#include <limits.h>
#include <stdint.h>

typedef uint8_t encoded_word;

#pragma pack(push) //pragma is needed to remove padding (extra spacing)
#pragma pack(1)
struct comms_Packet {
  //int8_t time;
  //int8_t data[3];
  float acc[3];
  int scale;

  uint32_t pressure;
  float temperature1;
  float altitude;
  uint32_t sealevel;

  float humidity;
  float temperature2;
  float compRH;

  struct tm time;
  struct gps_exttm exttm;
};
#pragma pack(pop)

char comms_address[20];
int comms_codelen; // 6 <= codelen <= 1 in bits

int comms_sendMessage(char *buffer, int len);
char *comms_receiveMessage(int len);

char *comms_EncodeMessage(struct comms_Packet pck);
struct comms_Packet comms_DecodeMessage(char *buffer);

char *comms_PackMessage(struct comms_Packet pck);
struct comms_Packet comms_UnpackMessage(char *values);

int comms_hammingdistance(encoded_word a, encoded_word b);
encoded_word *comms_hadamard(int comms_codelen);
gsl_matrix *comms_KPro(gsl_matrix *a, gsl_matrix *b);
int comms_printbin (encoded_word dec);