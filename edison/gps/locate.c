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
      gps_nmea("$GPRMC");
      gps_parse();
      gsl_vector *loc = gps_location;
      gsl_vector_fprintf(stdout, loc, "%f");

      exttm = gps_time;
      time = (struct tm){exttm.tm_sec, exttm.tm_min, exttm.tm_hour, exttm.tm_mday, exttm.tm_mon, exttm.tm_year, exttm.tm_wday, exttm.tm_yday};
      printf("The time is: %s\n",asctime(&time));

      printf("ground speed: %f\n", gps_speed);
      printf("course: %f\n", gps_course);
/*
      printf("fix qual: %d\n", gps_fix_quality);
      printf("sats : %d\n", gps_satelites);
      printf("hdop: %f\n", gps_hdop);
      printf("alt : %f\n", gps_altitude);
      printf("ellip : %f\n", gps_ellipsoid_seperation);
*/
    } else {
      printf("No fix\n");
      usleep(1000000);
    }
  }
}
