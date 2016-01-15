#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <mraa.h>

mraa_spi_context spi;
mraa_gpio_context gpio;

uint16_t mcp3204c_init() {

  spi = mraa_spi_init(0);
  if (spi == NULL) {
    printf("Initialization of spi failed, check syslog for details, exit...\n");
    exit(1);
  }

  mraa_spi_frequency(spi, 2000000);

  gpio = mraa_gpio_init(15);
  mraa_gpio_dir(gpio, MRAA_GPIO_OUT);
  mraa_gpio_write(gpio, 1);

}

uint16_t mcp3204c_read() {

  uint8_t high_input, low_input;
  high_input = 0b00000110;
  low_input = 0b00000000;
  uint8_t high_output, low_output;

  mraa_gpio_write(gpio, 0);

  mraa_spi_write(spi, high_input);
  high_output = mraa_spi_write(spi, low_input);
  low_output = mraa_spi_write(spi, 0xFF);

  mraa_gpio_write(gpio, 1);


  high_output &= 0b00001111;
  return (high_output<<8)|low_output;
}

uint16_t mcp3204c_stop() {
  mraa_spi_stop(spi);
  mraa_gpio_close(gpio);
}

int main() {

  uint16_t read;
  mcp3204c_init();
  while (1) {
    read = mcp3204c_read();
    printf("val is: %d\n", read);
    usleep(1000000);
  }
  mcp3204c_stop();

}
