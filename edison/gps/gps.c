#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <mraa.h>

#include "../exttm.h"
#include "gps.h"

int gps_init() {
  gps_uart = mraa_uart_init_raw("/dev/ttyMFD1");
  if (gps_uart == NULL) {
    fprintf(stderr, "UART failed to setup\n");
    return EXIT_FAILURE;
  }
  if (!mraa_uart_data_available(gps_uart, 2000)) {
    fprintf(stderr, "No data available\n");
    return EXIT_FAILURE;
  }
//  gps_gpio = mraa_gpio_init(48);
//  mraa_gpio_dir(gps_gpio, MRAA_GPIO_IN);
/*  char *cmd = "$PMTK220,100*2F";
  cmd[15] = <CR>;
  cmd[16] = <LF>;
  mraa_uart_write(gps_uart, cmd, 17);
*/
}

int gps_get_nmea(char *code) {
  char buffer;

  for (int i = 0; i < 1000; i++) { //this is abs max, incase gps is unplugged or broken
    mraa_uart_read(gps_uart, &buffer, sizeof(buffer));
    if (buffer == 10) { //checks for linefeed
      char line[100] = "";
      int i = 0;

      do { //ends at carriage return
        mraa_uart_read(gps_uart, &buffer, sizeof(buffer));
        line[i] = buffer;
        i++;
      } while (buffer != 10);

      char header[7];
      strncpy( header, line, sizeof(header));
      header[6] = '\0';

      if (strcmp(header,code) == 0) {
        strcpy(gps_line, line);
        if (gps_parse() == 3) return 3; // 3: no fix
        return EXIT_SUCCESS;
      }
    }
  }
  return EXIT_FAILURE;
}

int gps_parse() {
  char line[100];
  strcpy(line, gps_line);

  char header[7];
  strncpy(header, line, 7);
  header[6] = '\0';

  if (strcmp(header, "$GPRMC") == 0) {

    if ( line[18] == (int)'A') { //checks for fix

      struct exttm timeinfo;
      //get UTC D&T
      timeinfo.tm_hour = gps_chrtoint(line[7])*10 + gps_chrtoint(line[8]);
      timeinfo.tm_min = gps_chrtoint(line[9])*10 + gps_chrtoint(line[10]);
      timeinfo.tm_sec = gps_chrtoint(line[11])*10 + gps_chrtoint(line[12]);
      timeinfo.tm_msec = gps_chrtoint(line[14])*100 + gps_chrtoint(line[15])*10 + gps_chrtoint(line[16]);
      timeinfo.tm_mday = gps_chrtoint(line[57])*10 + gps_chrtoint(line[58]);
      timeinfo.tm_mon = (gps_chrtoint(line[59])*10 + gps_chrtoint(line[60]))-1;
      timeinfo.tm_year = (gps_chrtoint(line[61])*10 + gps_chrtoint(line[62]))+100;
      timeinfo.tm_wday = gps_wday(timeinfo.tm_year, timeinfo.tm_mon+1, timeinfo.tm_mday);
      timeinfo.tm_yday = gps_yday(timeinfo.tm_year, timeinfo.tm_mon+1, timeinfo.tm_mday);

      gps_time = timeinfo;

      gps_speed = (gps_chrtoint(line[45])+gps_chrtoint(line[47])*0.1+gps_chrtoint(line[48])*0.01)*0.514444444; // original unit is knots
      gps_course = gps_chrtoint(line[50])*100+gps_chrtoint(line[51])*10+gps_chrtoint(line[52])+gps_chrtoint(line[54])*0.1+gps_chrtoint(line[55])*0.01;

      float location[2];
      //get location data
      location[0] = gps_chrtoint(line[20])*1000+gps_chrtoint(line[21])*100+gps_chrtoint(line[22])*10+gps_chrtoint(line[23])+gps_chrtoint(line[25])*0.1+gps_chrtoint(line[26])*0.01+gps_chrtoint(line[27])*0.001+gps_chrtoint(line[28])*0.0001;
      if ( line[30] == (int)'S') {
        location[0] *= -1;
      }
      location[1] = gps_chrtoint(line[32])*10000+gps_chrtoint(line[33])*1000+gps_chrtoint(line[34])*100+gps_chrtoint(line[35])*10+gps_chrtoint(line[36])+gps_chrtoint(line[38])*0.1+gps_chrtoint(line[39])*0.01+gps_chrtoint(line[40])*0.001+gps_chrtoint(line[41])*0.0001;
      if (line[43] == (int)'W') {
        location[1] *= -1;
      }

      memcpy(gps_location, location, 2*sizeof(float));

      return EXIT_SUCCESS;

    } else {
      return 3; //No fix
    }
  } else if (strcmp(header, "$GPGGA") == 0) {

    // this method breaks if no fix

    // unused time data exists (no date)
    // unused location data exists

    gps_fix_quality = gps_chrtoint(line[43]);
    gps_satelites = gps_chrtoint(line[45]);

    gps_hdop = gps_chrtoint(line[47])+gps_chrtoint(line[49])*0.1+gps_chrtoint(line[50])*0.01;
    int i = 0;
    int fs = 0;
    if (line[52] == '-') {i++; fs=1;} // if negative, line is shifted and signed flipped
    gps_altitude = gps_chrtoint(line[52+i])*100+gps_chrtoint(line[53+i])*10+gps_chrtoint(line[54+i])+gps_chrtoint(line[56+i])*0.1;
    if (fs) gps_altitude *= -1; fs=0;
    if (line[60+i] == '-') {i++; fs=1;} // if negative, line is shifted and signed flipped
    gps_ellipsoid_seperation = gps_chrtoint(line[60+i])*10+gps_chrtoint(line[61+i])+gps_chrtoint(line[63+i])*0.1;
    if (fs) gps_ellipsoid_seperation *= -1; fs=0;

  }
}

int gps_chrtoint (char number) {
  if ((number >= 48)&&(number <= 57)) {
    return (int)(number - '0');
  } else {
    return EXIT_FAILURE;
  }
}

/*int gps_fix() {
  int val;
  for (int i = 0; i < 20000; i++) {
    val = mraa_gpio_read(gps_gpio);
    if (val == 1) {
      return 0; // false: no fix
      break;
    }
    usleep(100);
  }
  return 1; //true: fix
}*/

// http://rosettacode.org/wiki/Day_of_the_week#C
/* Calculate day of week in proleptic Gregorian calendar. Sunday == 0. */
// January == 1
int gps_wday(int year, int month, int day) {
  int adjustment, mm, yy;
  adjustment = (14 - month) / 12;
  mm = month + 12 * adjustment - 2;
  yy = year - adjustment;
  return (day + (13 * mm - 1) / 5 + yy + yy / 4 - yy / 100 + yy / 400) % 7;
}

// https://astronomy.stackexchange.com/questions/2407/calculate-day-of-the-year-for-a-given-date
// January == 1
int gps_yday(int year, int month, int day) {
  int N1, N2, N3, N;
  N1 = floor(275 * month / 9);
  N2 = floor((month + 9) / 12);
  N3 = (1 + floor((year - 4 * floor(year / 4) + 2) / 3));
  N = N1 - (N2 * N3) + day - 30;
  return N;
}
