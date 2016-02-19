#include <stdio.h>
#include <string.h>
#include <time.h>

#include "gps.h"
#include "../exttm.h"

int main() {
  struct tm time;
  struct gps_exttm exttm;
  float location[2];
  gps_init();

  while (1) {
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
      printf("The time is: %s\n",asctime(&time));
      printf("ground speed: %f\n", gps_speed);
      printf("course: %f\n", gps_course);
      printf("fix quality: %d\n", gps_fix_quality);
      printf("satelites: %d\n", gps_satelites);
      printf("horizontal dilution of precision: %f\n", gps_hdop);
      printf("altitude: %f\n", gps_altitude);
      printf("ellipsoid seperation: %f\n", gps_ellipsoid_seperation);

      printf("\n\n");

    } else printf("No fix\n");
    usleep(1000000);
  }
}
