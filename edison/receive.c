#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <gsl/gsl_vector.h>

#include "comms/comms.h"

int main() {

  comms_codelen = 4;
  // to change this, also change typedef for code

  strcpy(comms_address, "/dev/ttyACM0");
  struct comms_Packet pck;
  struct comms_Control ctrl;
  int tries;

  while(1) {
    pck = comms_UnpackMessage(comms_receiveMessage(sizeof(struct comms_Packet)));
    FILE *f;
    f = fopen("data.csv","w");
    fprintf(f, "%f,", pck.acc[0]);
    fprintf(f, "%f,", pck.acc[1]);
    fprintf(f, "%f,", pck.acc[2]);
    fprintf(f, "%d,", pck.scale);
    fprintf(f, "%d,", pck.pressure);
    fprintf(f, "%f,", pck.temperature1);
    fprintf(f, "%f,", pck.altitude1);
    fprintf(f, "%d,", pck.sealevel);
    fprintf(f, "%f,", pck.humidity);
    fprintf(f, "%f,", pck.temperature2);
    fprintf(f, "%f,", pck.compRH);
    fprintf(f, "%d,", pck.servo_ang);
    struct tm ascify = (struct tm){pck.time.tm_sec, pck.time.tm_min, pck.time.tm_hour, pck.time.tm_mday, pck.time.tm_mon, pck.time.tm_year, pck.time.tm_wday, pck.time.tm_yday};
    fprintf(f, "%s,", asctime(&ascify));
    fprintf(f, "%f,", gsl_vector_get(&pck.location,0));
    fprintf(f, "%f,", gsl_vector_get(&pck.location,1));
    fprintf(f, "%f,", pck.speed);
    fprintf(f, "%f,", pck.course);
    fprintf(f, "%d,", pck.fix_quality);
    fprintf(f, "%d,", pck.satelites);
    fprintf(f, "%f,", pck.hdop);
    fprintf(f, "%f,", pck.altitude2);
    fprintf(f, "%f\n", pck.ellipsoid_seperation);
    fclose(f);

    char *buf;
    f = fopen("servo.csv","w");
    fgets(buf, 4, f);
    fclose(f);

    ctrl.servo_ang = atoi(buf);
    if (comms_hamdist >= 4) {
      comms_codelen++;
      ctrl.codelen = comms_codelen;
      tries = 0;
    } else if (comms_hamdist == 0) {
      if (tries >= 3) { 
        comms_codelen--;
        ctrl.codelen = comms_codelen;
      }
      tries++;
    }

    usleep(1000000);
  }

  return 0;
}
