#include <stdio.h>
#include <time.h>
#include <gsl/gsl_vector.h>

#include "gps.h"

int main() {
  struct tm *tm;
  time_t time;
  gps_init();

  while (1) {
    if (gps_fix()) {
      gsl_vector *loc = gps_get_location();
      gsl_vector_fprintf(stdout, loc, "%f");

      time = gps_get_time();
      tm = gmtime(&time);
      printf("The time is: %s",asctime(tm));
    } else printf("No fix\n");
  }
}
