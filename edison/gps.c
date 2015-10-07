#include <stdio.h>
#include <mraa.h>
#include <time.h>
#include <string.h>

struct exttm { //just like inbuilt struct tm, but has milliseconds
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

struct location {
  float lat;          /* latitude, range 90 to -90        */
  float lon;          /* longitude, range 180 to -180     */
};

int main() {
  mraa_uart_context uart;
  uart = mraa_uart_init(0);
  if (uart == NULL) {
    fprintf(stderr, "UART failed to setup\n");
    return -1;
  }
  if (mraa_uart_data_available(uart, 5000)) {
    char buffer;
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
        char header[5];
        strncpy( header, line, sizeof(header));
        if (strcmp(header,"GPRMC") != 0) {
          struct tm *timeinfo;
          time_t rawtime;
          time(&rawtime);
          timeinfo = localtime (&rawtime);
          struct location loc;
          timeinfo->tm_hour = line[7]*10 + line[8];
          timeinfo->tm_min = line[9]*10 + line[10];
          timeinfo->tm_sec = line[11]*10 + line[12];
          //timeinfo->tm_msec = line[14]*100 + line[15]*10 + line[16];
          if ( line[18] == (int)"A") { //checks for fix
            loc.lat = line[20]*1000+line[21]*100+line[22]*10+line[23]+line[25]*0.1+line[26]*0.01+line[27]*0.001+line[28]*0.0001;
            if ( line[30] == (int)"S") {
              loc.lat *= -1;
            }
            loc.lon = line[32]*10000+line[33]*1000+line[34]*100+line[35]*10+line[36]+line[38]*0.1+line[39]*0.01+line[40]*0.001+line[41]*0.0001;
            if (line[43] == (int)"W") {
              loc.lat *= -1;
            }
            printf("location is: %f, %f", loc.lat, loc.lon);
          }
          printf("time is: %s", asctime(timeinfo));
        }
      }
    }
  } else {
    printf("unavailable\n");
  }
  mraa_uart_stop(uart);
  mraa_deinit();
  return 0;
}
