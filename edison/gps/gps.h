#include <stdio.h>
#include <mraa.h>
#include <time.h>
#include <string.h>
#include <gsl/gsl_vector.h>

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
  int tm_isdst;       /* daylight saving time             */
};

struct tm gps_returntime;
gsl_vector *gps_returnloc; //latitude then longitude
mraa_uart_context gps_uart;
mraa_gpio_context gps_gpio;
char gps_buffer;

int gps_init();
int gps_locate();
int gps_chrtoint (char number);
time_t gps_get_time();
gsl_vector *gps_get_location();
int gps_fix();