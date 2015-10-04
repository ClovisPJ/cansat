#include <stdio.h>
#include <mraa.h>

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
}

int main() {
{
  aa_uart_context uart;
  uart = mraa_uart_init(0);
  if (uart == NULL) {
    fprintf(stderr, "UART failed to setup\n");
    return -1;
  }
  char buffer;
  if (mraa_uart_data_available(uart, 5000)) {
    char buffer;
    char line[100]; //guess
    while 1 {
      mraa_uart_read(uart, buffer, sizeof(buffer));
      if (buffer == 10) { //checks for linefeed
        for (int i = 0; buffer != 13; i++) { //ends at carriage return
          mraa_uart_read(uart, buffer, sizeof(buffer));
          line[i] = buffer;
        }
      }
      //check for $GPRMC
      struct exttm time;
      time.hour = line[7]*10 + line[8];
      time.min = line[9]*10 + line[10];
      time.sec = line[11]*10 + line[12];
      time.msec = line[14]*100 + line[15]*10 + line[16];
      time.msec = line[14]*100 + line[15]*10 + line[16];
    }
  } else {
    printf("unavailable\n");
  }
  mraa_uart_stop(uart);
  mraa_deinit();
  return 0;
}
