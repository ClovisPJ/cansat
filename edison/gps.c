#include <stdio.h>
#include <mraa.h>
#include <time.h>
#include <string.h>
#include <gsl/gsl_vector.h>

#include "gps.h"

int gps_init() {
  uart = mraa_uart_init(0);
  returnloc = gsl_vector_calloc(2); 
  if (uart == NULL) {
    fprintf(stderr, "UART failed to setup\n");
    return EXIT_FAILURE;
  }
  if (!mraa_uart_data_available(uart, 5000)) {
    fprintf(stderr, "No data available\n");
    return EXIT_FAILURE;
  }
}

int gps_locate() {
  while (1) {
    mraa_uart_read(uart, &buffer, sizeof(buffer));
    if (buffer == 10) { //checks for linefeed
      char line[100] = ""; //guess
      int i = 0;
      do { //ends at carriage return
        mraa_uart_read(uart, &buffer, sizeof(buffer));
        line[i] = buffer;
        i++;
      } while (buffer != 10);
      char header[7];
      strncpy( header, line, sizeof(header));
      header[6] = '\0';
      if (strcmp(header,"$GPRMC") == 0) {
        struct tm *timeinfo;
        time_t rawtime;
        time(&rawtime);
        timeinfo = localtime (&rawtime);
        //struct location loc;
        gsl_vector *loc = gsl_vector_calloc(2);
        timeinfo->tm_hour = chrtoint(line[7])*10 + chrtoint(line[8]);
        timeinfo->tm_min = chrtoint(line[9])*10 + chrtoint(line[10]);
        timeinfo->tm_sec = chrtoint(line[11])*10 + chrtoint(line[12]);
        //timeinfo->tm_msec = chrtoint(line[14])*100 + chrtoint(line[15])*10 + chrtoint(line[16]);
        if ( line[18] == (int)'A') { //checks for fix
          gsl_vector_set(loc, 0, chrtoint(line[20])*1000+chrtoint(line[21])*100+chrtoint(line[22])*10+chrtoint(line[23])+chrtoint(line[25])*0.1+chrtoint(line[26])*0.01+chrtoint(line[27])*0.001+chrtoint(line[28])*0.0001);
          if ( line[30] == (int)'S') {
            gsl_vector_set(loc, 0, -1*gsl_vector_get(loc, 0));
          }
          gsl_vector_set(loc, 1, chrtoint(line[32])*10000+chrtoint(line[33])*1000+chrtoint(line[34])*100+chrtoint(line[35])*10+chrtoint(line[36])+chrtoint(line[38])*0.1+chrtoint(line[39])*0.01+chrtoint(line[40])*0.001+chrtoint(line[41])*0.0001);
          if (line[43] == (int)'W') {
            gsl_vector_set(loc, 1, -1*gsl_vector_get(loc, 1));
          }
          //printf("location is: %f, %f   ", loc.lat, loc.lon);
          //printf("time is: %s", asctime(timeinfo));
          gsl_vector_memcpy(returnloc, loc);
          returntime = *timeinfo;
          goto END;
        }
      }
    }
  }
  END:mraa_uart_stop(uart);
  mraa_deinit();
  return EXIT_SUCCESS;
}

int chrtoint (char number) {
  if ((number >= 48)&&(number <= 57)) {
    return (int)(number - '0');
  } else {
    return EXIT_FAILURE;
  }
}

struct tm gps_get_time() {
  return returntime;
}

gsl_vector *gps_get_location() {
  return returnloc;
}
