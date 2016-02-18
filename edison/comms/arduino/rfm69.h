#include <stdint.h>

int rfm69_CS = 10;

void rfm69_spi_setup();
uint8_t rfm69_read_reg(uint8_t addr);
void rfm69_write_reg(uint8_t addr, uint8_t val);
uint8_t *rfm69_read_fifo(uint8_t len);
void rfm69_write_fifo(uint8_t *data, uint8_t len);
void rfm69_settings();
void rfm69_send(char *data, int len);
char *rfm69_receive(int len);