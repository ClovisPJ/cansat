#include <stdio.h>
#include <time.h>
#include <string.h>
#include <gsl/gsl_vector.h>

#include <mraa.h>

#include "gps.h"

int gps_init() {
  gps_uart = mraa_uart_init(0);
  gps_returnloc = gsl_vector_calloc(2); 
  if (gps_uart == NULL) {
    fprintf(stderr, "UART failed to setup\n");
    return EXIT_FAILURE;
  }
  if (!mraa_uart_data_available(gps_uart, 2000)) {
    fprintf(stderr, "No data available\n");
    return EXIT_FAILURE;
  }
  gps_gpio = mraa_gpio_init(48);
  mraa_gpio_dir(gps_gpio, MRAA_GPIO_IN);
}

int gps_locate() {
  int tries = 0;
  while (1) {
    mraa_uart_read(gps_uart, &gps_buffer, sizeof(gps_buffer));
    if (gps_buffer == 10) { //checks for linefeed
      char line[100] = ""; //guess
      int i = 0;
      do { //ends at carriage return
        mraa_uart_read(gps_uart, &gps_buffer, sizeof(gps_buffer));
        line[i] = gps_buffer;
        i++;
      } while (gps_buffer != 10);
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
        timeinfo->tm_hour = gps_chrtoint(line[7])*10 + gps_chrtoint(line[8]);
        timeinfo->tm_min = gps_chrtoint(line[9])*10 + gps_chrtoint(line[10]);
        timeinfo->tm_sec = gps_chrtoint(line[11])*10 + gps_chrtoint(line[12]);
        //timeinfo->tm_msec = gps_chrtoint(line[14])*100 + gps_chrtoint(line[15])*10 + gps_chrtoint(line[16]);
        if ( line[18] == (int)'A') { //checks for fix
          gsl_vector_set(loc, 0, gps_chrtoint(line[20])*1000+gps_chrtoint(line[21])*100+gps_chrtoint(line[22])*10+gps_chrtoint(line[23])+gps_chrtoint(line[25])*0.1+gps_chrtoint(line[26])*0.01+gps_chrtoint(line[27])*0.001+gps_chrtoint(line[28])*0.0001);
          if ( line[30] == (int)'S') {
            gsl_vector_set(loc, 0, -1*gsl_vector_get(loc, 0));
          }
          gsl_vector_set(loc, 1, gps_chrtoint(line[32])*10000+gps_chrtoint(line[33])*1000+gps_chrtoint(line[34])*100+gps_chrtoint(line[35])*10+gps_chrtoint(line[36])+gps_chrtoint(line[38])*0.1+gps_chrtoint(line[39])*0.01+gps_chrtoint(line[40])*0.001+gps_chrtoint(line[41])*0.0001);
          if (line[43] == (int)'W') {
            gsl_vector_set(loc, 1, -1*gsl_vector_get(loc, 1));
          }
          //printf("location is: %f, %f   ", loc.lat, loc.lon);
          //printf("time is: %s", asctime(timeinfo));
          gsl_vector_memcpy(gps_returnloc, loc);
          gps_returntime = *timeinfo;
          return EXIT_SUCCESS;
	  break;
        } else {
	  if (tries >= 5) {
	    return 3; // 3: No fix
	    break;
	  } else tries++;
	}
      }
    }
  }
}

int gps_chrtoint (char number) {
  if ((number >= 48)&&(number <= 57)) {
    return (int)(number - '0');
  } else {
    return EXIT_FAILURE;
  }
}

time_t gps_get_time() {
  return mktime(&gps_returntime);
}

gsl_vector *gps_get_location() {
  return gps_returnloc;
}

int gps_fix() {
  int val;
  for (int i = 0; i < 100; i++) {
    val = mraa_gpio_read(gps_gpio);
    if (val == 1) {
      return 0; 
      break;
    }
    usleep(10000);
  }
  return 1;
}
