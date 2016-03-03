#define ADS1115_ADDRESS 0x48

#include <mraa.h>

int ads1115_init(int pin);
double ads1115_read();
int ads1115_stop();

mraa_i2c_context ads1115_i2c;
