#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <gsl/gsl_vector.h>

#include "accel/adxl345.h"
#include "baro/bmpx8x.h"
#include "humid/htu21d.h"
#include "gps/gps.h"
#include "comms.h"

int main (int argc, char **argv) {

    struct comms_Packet pck;

    float acc[3];
    int scale;
    adxl345_init();

    uint32_t pressure = 0;
    float temperature1 = 0;
    float altitude = 0;
    uint32_t sealevel = 0;
    bmpx8x_init(1, BMP085_ADDR, 3);

    float humidity    = 0.0;
    float temperature2 = 0.0;
    float compRH      = 0.0;
    htu21d_init(1, HTU21D_I2C_ADDRESS);
    htu21d_testsensor();

    struct tm time;
    struct gps_exttm exttm;
    gps_init();

    comms_codelen = 4;
    strcpy(comms_address, "/dev/ttyACM0");

    while(1) {

      adxl345_update();
      pck.acc = adxl345_getacceleration(); // Read acceleration (g) (+-1%)
      pck.scale = adxl345_getscale();

      pck.pressure = bmpx8x_getpressure (); //SD: 4 Pa
      pck.temperature1 = bmpx8x_gettemperature (); // SD: 5/30 degC
      pck.altitude = bmpx8x_getaltitude (101325); // SD: 1/3 degC
      pck.sealevel = bmpx8x_getsealevelpressure (0); // SD: 4 Pa

      pck.compRH      = htu21d_getcompRH(1);
      pck.humidity    = htu21d_gethumidity(0);
      pck.temperature2 = htu21d_gettemperature(0);

      if (gps_fix()) {
        gps_get_nmea("$GPRMC");
        gps_get_nmea("$GPGGA");

        gsl_vector *loc = gps_location;
        gsl_vector_memcpy(pck.loc, loc);

        exttm = gps_time;
        pck.exttm = exttm;
        time = (struct tm){exttm.tm_sec, exttm.tm_min, exttm.tm_hour, exttm.tm_mday, exttm.tm_mon, exttm.tm_year, exttm.tm_wday, exttm.tm_yday};

        pck.gps_speed = gps_speed;
        pck.gps_course = gps_course;
        pck.gps_fix_quality = gps_fix_quality;
        pck.gps_satelites = gps_satelites;
        pck.gps_hdop = gps_hdop;
        pck.gps_altitude = gps_altitude;
        pck.gps_ellipsoid_seperation = gps_ellipsoid_seperation;

      } else printf("No fix\n");
  
      fprintf(stdout, "Current scale: 0x%2xg\n", scale);
      fprintf(stdout, "AccX: %5.2f g\n", acc[0]); 
      fprintf(stdout, "AccY: %5.2f g\n", acc[1]);
      fprintf(stdout, "AccZ: %5.2f g\n", acc[2]);
      printf("pressure value = %d", pressure);
      printf("altitude value = %f", altitude);
      printf("sealevel value = %d", sealevel);
      printf("temperature = %f\n", temperature1);
      printf("humidity value = %f\n", humidity); //+- 2%
      printf("temperature value = %f\n", temperature2); //SD: 0.1 degC
      printf("compensated RH value = %f\n", compRH);
      gsl_vector_fprintf(stdout, loc, "%f");
      printf("The time is: %s\n",asctime(&time));
      printf("ground speed: %f\n", gps_speed);
      printf("course: %f\n", gps_course);
      printf("course: %f\n", gps_course);
      printf("fix quality: %d\n", gps_fix_quality);
      printf("satelites: %d\n", gps_satelites);
      printf("horizontal dilution of precision: %f\n", gps_hdop);
      printf("altitude: %f\n", gps_altitude);
      printf("ellipsoid seperation: %f\n", gps_ellipsoid_seperation);

      comms_sendMessage(comms_PackMessage(pck), sizeof(struct comms_Packet));
      pck = comms_UnpackMessage(comms_receiveMessage(sizeof(struct comms_Packet)));

      int comms_pckcodes = (sizeof(struct comms_Packet)*CHAR_BIT)/comms_codelen;
      int encodedwordlen = pow(2, comms_codelen-1);

      comms_sendMessage(comms_EncodeMessage(conv.values), (encodedwordlen*comms_pckcodes)/CHAR_BIT);
      pck = comms_DecodeMessage(comms_receiveMessage((encodedwordlen*comms_pckcodes)/CHAR_BIT))

    }
    return EXIT_SUCCESS;
}
