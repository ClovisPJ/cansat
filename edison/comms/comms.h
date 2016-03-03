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
  uint8_t scale;

  uint32_t pressure;
  float temperature1;
  float altitude1;
  uint32_t sealevel;

  float humidity;
  float temperature2;
  float compRH;

  float gas1;
  float gas2;
  
  int servo_ang;

 // struct tm time;
  struct exttm time; // 12 bytes

  float location[2];
  float speed;
  float course;
  uint8_t fix_quality;
  uint8_t satelites;
  float hdop;
  float altitude2;
  float ellipsoid_seperation;

};

struct comms_Control {
  int servo_ang;
  uint8_t codelen;
};
#pragma pack(pop)

int comms_codelen;
int comms_hamdist;

char *comms_PackMessage(struct comms_Packet pck);
struct comms_Packet comms_UnpackMessage(char *values);

char *comms_PackControl(struct comms_Control ctrl);
struct comms_Control comms_UnpackControl(char *values);

char *comms_EncodeMessage(struct comms_Packet pck);
struct comms_Packet comms_DecodeMessage(char *buffer);

int comms_hammingdistance(encoded_word a, encoded_word b);
int **comms_hadamard(int comms_codelen);
gsl_matrix *comms_KPro(gsl_matrix *a, gsl_matrix *b);
int comms_printbin (encoded_word dec);
