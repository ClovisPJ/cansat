#define ADS1115_ADDRESS 0x48

#include <mraa.h>

int ads1115_init();
double ads1115_read();
int ads1115_stop();

mraa_i2c_context i2c;
