#include <stdint.h>
#include <mraa.h>

#include "ads1115.h"

int ads1115_init(int pin) {
  ads1115_i2c = mraa_i2c_init(1);
  mraa_i2c_address(ads1115_i2c, ADS1115_ADDRESS);

  uint8_t write[3];
  write[0] = 1;
  write[1] = 0b10000010;
  if (pin == 0) write[1] += 0b01000000;
  else if (pin == 1) write[1] += 0b01010000;
  else return -1;
  write[2] = 0b00000011;

  uint8_t read[2];
  read[0] = 0;
  read[1] = 0;

  mraa_i2c_write(ads1115_i2c, write, 3);

  while ((read[0] & 0b1000000) == 0) {
    mraa_i2c_read(ads1115_i2c, read, 2);
  }

  write[0] = 0;
  mraa_i2c_write(ads1115_i2c, write, 1);

}

float ads1115_read() {
  float val;
  uint8_t read[2];
  mraa_i2c_read(ads1115_i2c, read, 2);
  val = read[0] << 8 | read[1];
  return (float)(val*4.096/32767.0); 
}

int ads1115_stop() {
  mraa_i2c_stop(ads1115_i2c);
}
