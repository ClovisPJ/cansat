#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include "accel/adxl345.h"
#include "baro/bmpx8x.h"
#include "humid/htu21d.h"
#include "adc/ads1115.h"
#include "servo/servo.h"
#include "gps/gps.h"
#include "exttm.h"

int main (int argc, char **argv) {

    int16_t *raw;
    float *acc;
    // Note: Sensor only works at 3.3V on the Intel Edison with Arduino breakout
    adxl345_init();

    uint32_t pressure = 0;
    float temperature1 = 0;
    float altitude = 0;
    uint32_t sealevel = 0;
    // Instantiate a BMPX8X sensor on I2C
    bmpx8x_init(1, BMP085_ADDR, 3);

    float humidity    = 0.0;
    float temperature2 = 0.0;
    float compRH      = 0.0;
    htu21d_init(1, HTU21D_I2C_ADDRESS);
    htu21d_testsensor();

    double gas1;
    double gas2;

    int ang;
    servo_init();

    struct tm time;
    struct exttm exttm;
    float location[2];
    gps_init();

    while(1) {

      adxl345_update(); // Update the data
      //raw = adxl345_getrawvalues(); // Read raw sensor data
      acc = adxl345_getacceleration(); // Read acceleration (g) (+-1%)
      //fprintf(stdout, "Raw: %6d %6d %6d\n", raw[0], raw[1], raw[2]);
      printf("AccX: %5.2f g\n", acc[0]); 
      printf("AccY: %5.2f g\n", acc[1]);
      printf("AccZ: %5.2f g\n", acc[2]);
      printf("Current scale: 0x%2xg\n", adxl345_getscale());

      pressure = bmpx8x_getpressure (); //SD: 4 Pa
      temperature1 = bmpx8x_gettemperature (); // SD: 5/30 degC
      altitude = bmpx8x_getaltitude (101325); // SD: 1/3 degC
      sealevel = bmpx8x_getsealevelpressure (0); // SD: 4 Pa
      printf("pressure value = %d, altitude value = %f, sealevel value = %d, temperature = %f\n", pressure, altitude, sealevel, temperature1);

      compRH      = htu21d_getcompRH(1);
      humidity    = htu21d_gethumidity(0);
      temperature2 = htu21d_gettemperature(0);
      printf("humidity value = %f\n", humidity); //+- 2%
      printf("temperature value = %f\n", temperature2); //SD: 0.1 degC
      printf("compensated RH value = %f\n", compRH);

      ads1115_init(0);
      gas1 = ads1115_read();
      ads1115_init(1);
      gas2 = ads1115_read();

      printf("AIN0 is: %fV\n", gas1);
      printf("AIN1 is: %fV\n", gas2);

      ang = servo_ang;      
      printf("servo angle: %d\n", ang);

      if (gps_fix()) {
        int ret = gps_get_nmea("$GPRMC");
        if (ret == 3) printf("No fix (wire says otherwise)\n");
        if (ret == -1) printf("NMEA sentence not receieved (wire unplugged?)\n");
        gps_get_nmea("$GPGGA");

        memcpy(location, gps_location, 2*sizeof(float));
        exttm = gps_time;
        time = (struct tm){exttm.tm_sec, exttm.tm_min, exttm.tm_hour, exttm.tm_mday, exttm.tm_mon, exttm.tm_year, exttm.tm_wday, exttm.tm_yday};

        printf("latitude is: %f\n", location[0]);
        printf("longitude is: %f\n", location[1]);
        printf("The time is: %s",asctime(&time));
        printf("ground speed: %f\n", gps_speed);
        printf("course: %f\n", gps_course);
        printf("fix quality: %d\n", gps_fix_quality);
        printf("satelites: %d\n", gps_satelites);
        printf("horizontal dilution of precision: %f\n", gps_hdop);
        printf("altitude: %f\n", gps_altitude);
        printf("ellipsoid seperation: %f\n", gps_ellipsoid_seperation);


      } else printf("No fix\n");

      printf("\n\n");
      usleep(1000000);

    }
    return EXIT_SUCCESS;
}
