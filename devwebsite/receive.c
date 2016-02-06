#include <stdio.h>
#include <string.h>
#include <time.h>
#include <gsl/gsl_vector.h>

#include "comms/comms.h"

int main() {

  comms_codelen = 4;
  // to change this, also change typedef for code
  strcpy(comms_address, "/dev/ttyACM0");
  struct comms_Packet pck;

  while(1) {
    pck = comms_UnpackMessage(comms_receiveMessage(sizeof(struct comms_Packet)));
    FILE *f;
    f = fopen("tmp/acc0", "w"); fprintf(f, "%f\n", pck.acc[0]); fclose(f);
    f = fopen("tmp/acc1", "w"); fprintf(f, "%f\n", pck.acc[1]); fclose(f);
    f = fopen("tmp/acc2", "w"); fprintf(f, "%f\n", pck.acc[2]); fclose(f);
    f = fopen("tmp/scale", "w"); fprintf(f, "%d\n", pck.scale); fclose(f);
    f = fopen("tmp/pressure", "w"); fprintf(f, "%d\n", pck.pressure); fclose(f);
    f = fopen("tmp/temperature1", "w"); fprintf(f, "%f\n", pck.temperature1); fclose(f);
    f = fopen("tmp/altitude1", "w"); fprintf(f, "%f\n", pck.altitude1); fclose(f);
    f = fopen("tmp/sealevel", "w"); fprintf(f, "%d\n", pck.sealevel); fclose(f);
    f = fopen("tmp/humidity", "w"); fprintf(f, "%f\n", pck.humidity); fclose(f);
    f = fopen("tmp/temperature2", "w"); fprintf(f, "%f\n", pck.temperature2); fclose(f);
    f = fopen("tmp/compRH", "w"); fprintf(f, "%f\n", pck.compRH); fclose(f);
    f = fopen("tmp/time", "w");
    struct tm ascify = (struct tm){pck.time.tm_sec, pck.time.tm_min, pck.time.tm_hour, pck.time.tm_mday, pck.time.tm_mon, pck.time.tm_year, pck.time.tm_wday, pck.time.tm_yday};
    fprintf(f, "%s\n", asctime(&ascify)); fclose(f);
    f = fopen("tmp/location0", "w"); fprintf(f, "%f\n", gsl_vector_get(&pck.location,0)); fclose(f);
    f = fopen("tmp/location1", "w"); fprintf(f, "%f\n", gsl_vector_get(&pck.location,1)); fclose(f);
    f = fopen("tmp/speed", "w"); fprintf(f, "%f\n", pck.speed); fclose(f);
    f = fopen("tmp/course", "w"); fprintf(f, "%f\n", pck.course); fclose(f);
    f = fopen("tmp/fix_quality", "w"); fprintf(f, "%d\n", pck.fix_quality); fclose(f);
    f = fopen("tmp/satelites", "w"); fprintf(f, "%d\n", pck.satelites); fclose(f);
    f = fopen("tmp/hdop", "w"); fprintf(f, "%f\n", pck.hdop); fclose(f);
    f = fopen("tmp/altitude2", "w"); fprintf(f, "%f\n", pck.altitude2); fclose(f);
    f = fopen("tmp/ellipsoid_seperation", "w"); fprintf(f, "%f\n", pck.ellipsoid_seperation); fclose(f);

    usleep(1000000);
  }

  return 0;
}
