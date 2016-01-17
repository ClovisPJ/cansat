#include <stdint.h>
#include <mraa.h>

#include "ads.h"

int ads1115_init() {
  i2c = mraa_i2c_init(1);
  mraa_i2c_address(i2c, ADS1115_ADDRESS);

  uint8_t write[3];
  write[0] = 1;
  write[1] = 0b11000010;
  write[2] = 0b00000011;

  uint8_t read[2];
  read[0] = 0;
  read[1] = 0;

  mraa_i2c_write(i2c, write, 3);

  while ((read[0] & 0b1000000) == 0) {
    mraa_i2c_read(i2c, read, 2);
  }

  write[0] = 0;
  mraa_i2c_write(i2c, write, 1);

}

double ads1115_read() {
  double val;
  uint8_t read[2];
  mraa_i2c_read(i2c, read, 2);
  val = read[0] << 8 | read[1];
  return (float)(val*4.096/32767.0); 
}

int ads1115_stop() {
  mraa_i2c_stop(i2c);
}
