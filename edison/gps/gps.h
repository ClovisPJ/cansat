#include <stdio.h>
#include <string.h>
#include <gsl/gsl_vector.h>
#include <time.h>

#include <mraa.h>

struct gps_exttm { //just like inbuilt struct tm, but has milliseconds
  int tm_msec;        /* milliseconds,  range 0 to 999    */
  int tm_sec;         /* seconds,  range 0 to 59          */
  int tm_min;         /* minutes, range 0 to 59           */
  int tm_hour;        /* hours, range 0 to 23             */
  int tm_mday;        /* day of the month, range 1 to 31  */
  int tm_mon;         /* month, range 0 to 11             */
  int tm_year;        /* The number of years since 1900   */
  int tm_wday;        /* day of the week, range 0 to 6    */
  int tm_yday;        /* day in the year, range 0 to 365  */
};

mraa_uart_context gps_uart;
mraa_gpio_context gps_gpio;

char gps_line[100];
struct gps_exttm gps_time;
gsl_vector *gps_location;
float gps_speed; // over ground, m/s
float gps_course; // made good, true, degrees
int gps_fix_quality; // 0 - no fix, 1 - GPS fix, 2 - DGPS fix
int gps_satelites;
float gps_hdop; // horizontal dilution of precision
float gps_altitude; // metres
float gps_ellipsoid_seperation; // height above WGS84 ellipsoid, metres

int gps_init();
int gps_get_nmea(char *code);
int gps_parse();

int gps_chrtoint (char number);
int gps_fix();

int gps_wday(int year, int month, int day);
int gps_yday(int year, int month, int day);
