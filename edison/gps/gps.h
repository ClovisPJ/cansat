#include <stdio.h>
#include <string.h>
#include <time.h>

#include <mraa.h>

mraa_uart_context gps_uart;
mraa_gpio_context gps_gpio;

char gps_line[100];
struct gps_exttm gps_time;
float gps_location[2];
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
