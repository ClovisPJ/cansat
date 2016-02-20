#pragma once
struct exttm { // just like inbuilt struct tm, but has milliseconds
  uint16_t tm_msec;        /* milliseconds,  range 0 to 999    */
  uint8_t tm_sec;         /* seconds,  range 0 to 59          */
  uint8_t tm_min;         /* minutes, range 0 to 59           */
  uint8_t tm_hour;        /* hours, range 0 to 23             */
  uint8_t tm_mday;        /* day of the month, range 1 to 31  */
  uint8_t tm_mon;         /* month, range 0 to 11             */
  uint8_t  tm_year;        /* The number of years since 1900   */  // this will break in 2156
  uint8_t tm_wday;        /* day of the week, range 0 to 6    */
  uint16_t tm_yday;        /* day in the year, range 0 to 365  */
}; // 11 bytes
