#include <time.h> 
#include <gsl/gsl_matrix.h>
#include <limits.h>
#include <stdint.h>

#include "../exttm.h"

typedef uint8_t encoded_word;

#pragma pack(push) //pragma is needed to remove padding (extra spacing)
#pragma pack(1)
struct comms_Packet {
  float acc[3];
  int scale;

  uint32_t pressure;
  float temperature1;
  float altitude1;
  uint32_t sealevel;

  float humidity;
  float temperature2;
  float compRH;

 // struct tm time;
  struct gps_exttm time;

  float location[2];
  float speed;
  float course;
  int fix_quality;
  int satelites;
  float hdop;
  float altitude2;
  float ellipsoid_seperation;
};
#pragma pack(pop)

int comms_codelen; // 6 <= codelen <= 1 in bits

char *comms_EncodeMessage(struct comms_Packet pck);
struct comms_Packet comms_DecodeMessage(char *buffer);

char *comms_PackMessage(struct comms_Packet pck);
struct comms_Packet comms_UnpackMessage(char *values);

int comms_hammingdistance(encoded_word a, encoded_word b);
int **comms_hadamard(int comms_codelen);
gsl_matrix *comms_KPro(gsl_matrix *a, gsl_matrix *b);
int comms_printbin (encoded_word dec);
