#include <stdint.h>
#include <mraa.h>

mraa_spi_context rfm69_spi;

void rfm69_spi_setup();
uint8_t rfm69_read_reg(uint8_t addr);
void rfm69_write_reg(uint8_t addr, uint8_t val);
void rfm69_settings();
void rfm69_send(char* data, int len);
char *rfm69_receive(int len);
uint8_t rfm69_temperature();
