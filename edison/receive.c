#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <gsl/gsl_vector.h>

#include "comms/comms.h"
#include "comms/uart/uart.h"

int main() {

  comms_codelen = 4;
  // to change this, also change typedef for code

  strcpy(uart_address, "/dev/ttyACM0\0");
  struct comms_Packet pck;
//  struct comms_Control ctrl;
//  int tries;

  while(1) {
    pck = comms_UnpackMessage(uart_receiveMessage(sizeof(struct comms_Packet)));
    FILE *f;
    f = fopen("/home/clovis/cansat/cansat/devwebsite/data.csv","a");
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
    fprintf(f, "%f,", pck.gas1);
    fprintf(f, "%f,", pck.gas2);
    fprintf(f, "%d,", pck.servo_ang);
    struct tm mkify = (struct tm){pck.time.tm_sec, pck.time.tm_min, pck.time.tm_hour, pck.time.tm_mday, pck.time.tm_mon, pck.time.tm_year, pck.time.tm_wday, pck.time.tm_yday};
    fprintf(f, "%d,", (uint32_t)(mktime(&mkify)));
    fprintf(f, "%f,", pck.location[0]);
    fprintf(f, "%f,", pck.location[1]);
    fprintf(f, "%f,", pck.speed);
    fprintf(f, "%f,", pck.course);
    fprintf(f, "%d,", pck.fix_quality);
    fprintf(f, "%d,", pck.satelites);
    fprintf(f, "%f,", pck.hdop);
    fprintf(f, "%f,", pck.altitude2);
    fprintf(f, "%f,", pck.ellipsoid_seperation);
    fprintf(f, "%d\n", pck.temperature3);
    fclose(f);

    printf("AccX: %5.2f g\n", pck.acc[0]); 
    printf("AccY: %5.2f g\n", pck.acc[1]);
    printf("AccZ: %5.2f g\n", pck.acc[2]);
    printf("Current scale: 0x%2xg\n", pck.scale);
    printf("pressure value = %d", pck.pressure);
    printf("altitude value = %f", pck.altitude1);
    printf("sealevel value = %d", pck.sealevel);
    printf("temperature = %f\n", pck.temperature1);
    printf("humidity value = %f\n", pck.humidity); //+- 2%
    printf("temperature value = %f\n", pck.temperature2); //SD: 0.1 degC
    printf("compensated RH value = %f\n", pck.compRH);
    printf("AIN0 is: %fV\n", pck.gas1);
    printf("AIN1 is: %fV\n", pck.gas2);
    printf("servo angle: %d\n", pck.servo_ang);
    printf("latitude is: %f\n", pck.location[0]);
    printf("longitude is: %f\n", pck.location[1]);
    struct tm ascify = (struct tm){pck.time.tm_sec, pck.time.tm_min, pck.time.tm_hour, pck.time.tm_mday, pck.time.tm_mon, pck.time.tm_year, pck.time.tm_wday, pck.time.tm_yday};
    printf("The time is: %s\n",asctime(&ascify));
    printf("ground speed: %f\n", pck.speed);
    printf("course: %f\n", pck.course);
    printf("fix quality: %d\n", pck.fix_quality);
    printf("satelites: %d\n", pck.satelites);
    printf("horizontal dilution of precision: %f\n", pck.hdop);
    printf("altitude: %f\n", pck.altitude2);
    printf("ellipsoid seperation: %f\n", pck.ellipsoid_seperation);
    printf("temperature from radio: %d\n", pck.temperature3);

/*    char *buf;
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

    char *p = comms_PackControl(ctrl);
    uart_sendMessage(p, sizeof(struct comms_Control);
    free(p);
*/

    usleep(5000000);
  }

  return 0;
}
