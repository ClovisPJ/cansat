#include <stdio.h>
#include <gsl/gsl_vector.h>

#include "gps.h"

int main() {

  gps_init();
  gps_locate();

  gsl_vector *loc;
  loc = gps_get_location();
  gsl_vector_fprintf(stdout, loc, "%f");

}
