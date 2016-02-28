#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "accel/adxl345.h"
#include "baro/bmpx8x.h"
#include "humid/htu21d.h"
#include "gps/gps.h"
#include "comms/comms.h"
#include "comms/edison/rfm69.h"

int main (int argc, char **argv) {

    struct comms_Packet pck;

    adxl345_init();
    bmpx8x_init(1, BMP085_ADDR, 3);
    htu21d_init(1, HTU21D_I2C_ADDRESS);
    htu21d_testsensor();
    gps_init();

    comms_codelen = 4;
    rfm69_settings();

    while(1) {

      adxl345_update();
      float *acc = adxl345_getacceleration(); // Read acceleration (g) (+-1%)
      memcpy(pck.acc, acc, 3*sizeof(float));
      pck.scale = adxl345_getscale();

      pck.pressure = bmpx8x_getpressure (); //SD: 4 Pa
      pck.temperature1 = bmpx8x_gettemperature (); // SD: 5/30 degC
      pck.altitude1 = bmpx8x_getaltitude (101325); // SD: 1/3 degC
      pck.sealevel = bmpx8x_getsealevelpressure (0); // SD: 4 Pa

      pck.compRH      = htu21d_getcompRH(1);
      pck.humidity    = htu21d_gethumidity(0);
      pck.temperature2 = htu21d_gettemperature(0);

      if (gps_fix()) {
        int ret = gps_get_nmea("$GPRMC");
        if (ret == 3) printf("No fix (wire says otherwise)\n");
        if (ret == -1) printf("NMEA sentence not receieved (wire unplugged?)\n");
        gps_get_nmea("$GPGGA");

        memcpy(pck.location, gps_location, 2*sizeof(float));
        pck.time = gps_time;
        //struct exttm exttm = gps_time;
        //pck.time = (struct tm){exttm.tm_sec, exttm.tm_min, exttm.tm_hour, exttm.tm_mday, exttm.tm_mon, exttm.tm_year, exttm.tm_wday, exttm.tm_yday};

        pck.speed = gps_speed;
        pck.course = gps_course;
        pck.fix_quality = gps_fix_quality;
        pck.satelites = gps_satelites;
        pck.hdop = gps_hdop;
        pck.altitude2 = gps_altitude;
        pck.ellipsoid_seperation = gps_ellipsoid_seperation;

      } else printf("No fix\n");
  
      fprintf(stdout, "AccX: %5.2f g\n", pck.acc[0]); 
      fprintf(stdout, "AccY: %5.2f g\n", pck.acc[1]);
      fprintf(stdout, "AccZ: %5.2f g\n", pck.acc[2]);
      fprintf(stdout, "Current scale: 0x%2xg\n", pck.scale);
      printf("pressure value = %d", pck.pressure);
      printf("altitude value = %f", pck.altitude1);
      printf("sealevel value = %d", pck.sealevel);
      printf("temperature = %f\n", pck.temperature1);
      printf("humidity value = %f\n", pck.humidity); //+- 2%
      printf("temperature value = %f\n", pck.temperature2); //SD: 0.1 degC
      printf("compensated RH value = %f\n", pck.compRH);
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

      printf("\n\n");

      rfm69_send(comms_PackMessage(pck), sizeof(struct comms_Packet));

//      int comms_pckcodes = (sizeof(struct comms_Packet)*CHAR_BIT)/comms_codelen;
//      int encodedwordlen = pow(2, comms_codelen-1);

//      rfm69_send(comms_EncodeMessage(conv.values), (encodedwordlen*comms_pckcodes)/CHAR_BIT);
      usleep(1000000);

    }
    return EXIT_SUCCESS;
}
