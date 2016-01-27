#include <stdio.h>
#include <time.h>
#include <gsl/gsl_vector.h>

#include "gps.h"

int main() {
  struct tm time;
  struct gps_exttm exttm;
  gps_init();

  while (1) {
    if (gps_fix()) {
      gps_get_nmea("$GPRMC");
      gps_get_nmea("$GPGGA");

      gsl_vector *loc = gps_location;
      exttm = gps_time;
      time = (struct tm){exttm.tm_sec, exttm.tm_min, exttm.tm_hour, exttm.tm_mday, exttm.tm_mon, exttm.tm_year, exttm.tm_wday, exttm.tm_yday};

      gsl_vector_fprintf(stdout, loc, "%f");
      printf("The time is: %s\n",asctime(&time));
      printf("ground speed: %f\n", gps_speed);
      printf("course: %f\n", gps_course);
      printf("fix quality: %d\n", gps_fix_quality);
      printf("satelites: %d\n", gps_satelites);
      printf("horizontal dilution of precision: %f\n", gps_hdop);
      printf("altitude: %f\n", gps_altitude);
      printf("ellipsoid seperation: %f\n", gps_ellipsoid_seperation);

    } else {
      printf("No fix\n");
      usleep(1000000);
    }
  }
}
