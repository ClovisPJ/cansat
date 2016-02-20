#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <mraa.h>

#include "rfm69.h"

void rfm69_spi_setup() {
  rfm69_spi = mraa_spi_init(0);
  mraa_spi_frequency(rfm69_spi, 100000);
  mraa_spi_mode(rfm69_spi, MRAA_SPI_MODE0);
}

uint8_t rfm69_read_reg(uint8_t addr) {
  uint8_t *in = calloc(2, sizeof(uint8_t));
  *in = addr;
  uint8_t *out = calloc(2, sizeof(uint8_t));
  mraa_spi_transfer_buf(rfm69_spi, in, out, 2);
  return *(out+1);
}

void rfm69_write_reg(uint8_t addr, uint8_t val) {
  uint8_t *in = calloc(2, sizeof(uint8_t));
  *in = 0b10000000 | addr; // begin 1 for write
  *(in+1) = val;
  mraa_spi_write_buf(rfm69_spi, in, 2);
}

void rfm69_settings() {
  rfm69_spi_setup();

  // set frequency
  /* Fcar = 433.35Mhz
   * Frf = Fstep / Fcar
   * Fstep = Fxosc / (2^19)
   * Fxosc = 32Mhz
   */
  uint32_t freq = 7100006;
  rfm69_write_reg(0x07, (freq>>16) & 0xFF);
  rfm69_write_reg(0x08, (freq>>8) & 0xFF);
  rfm69_write_reg(0x09, freq & 0xFF);

  // TxStartCondition -> FifoNotEmpty
  rfm69_write_reg(0x3C, 0b10001111);

  // Sync
  rfm69_write_reg(0x2C, 0x00);
  rfm69_write_reg(0x2D, 0x03);
  rfm69_write_reg(0x2E, 0b10111000);
  rfm69_write_reg(0x2F, (int)'$');
  rfm69_write_reg(0x30, (int)'$');
  rfm69_write_reg(0x31, (int)'Z');
  rfm69_write_reg(0x32, (int)'Y');
  rfm69_write_reg(0x33, (int)'S');
  rfm69_write_reg(0x34, (int)'K');
  rfm69_write_reg(0x35, (int)'$');
  rfm69_write_reg(0x36, (int)'$');
  // rfm69_write_reg(0x2E, 0b00101101);

  rfm69_write_reg(0x37, 0b00000000);
}

void rfm69_send(char *data, int len) {
  rfm69_spi_setup();
  uint8_t buf;

  // Packet length
  rfm69_write_reg(0x38, len);

  // Standby mode
  buf = rfm69_read_reg(0x01); // Mode Register
  buf &= 0b11100011; // keep other register variables
  buf += 0b00000100; // Mode -> Standby
  rfm69_write_reg(0x01, buf);

  // wait until Ready
  while ((rfm69_read_reg(0x27) & 0b10000000) == 0); // until crystal oscillator is running

  // format transmission data
  uint8_t *in = calloc(len+1, sizeof(uint8_t));
  in[0] = 0x00 | 0b10000000; // FIFO Register, begin 1 for write
  memcpy(in+1, data, len);

  // 66 is max size of FIFO register
  int large = 0;
  if (len > 66) large = 1;
  
  // write data to FIFO register (or first 66 bytes of)
  mraa_spi_write_buf(rfm69_spi, in, (large ? 66+1 : len+1) );

  // TX mode - sends data
  buf = rfm69_read_reg(0x01); // Mode Register
  buf &= 0b11100011; // keep other register variables
  buf += 0b00001100; // Mode -> TX
  rfm69_write_reg(0x01, buf);

  // if more data than FIFO size, fill "on-the-fly"
  uint8_t buf2[2];
  if (large == 1) {
    for (int i = 66; i < len; ) {
      while ((rfm69_read_reg(0x28) & 0b01000000) == 0) { // FIFO not empty
        while ( ((rfm69_read_reg(0x28) & 0b10000000) == 0) && (i < len)) { // FIFO not full
          buf2[0] = 0x00 | 0b10000000; // FIFO Register, begin 1 for write
          buf2[1] = data[i];
          mraa_spi_write_buf(rfm69_spi, buf2, 2);
          i++;
        }
      }
    }
  }

  // wait until Ready
  while ((rfm69_read_reg(0x28) & 0b00001000) == 0); // until packet sent

  // Standby mode
  buf = rfm69_read_reg(0x01); // Mode Register
  buf &= 0b11100011; // keep other register variables
  buf += 0b00000100; // Mode -> Standby
  rfm69_write_reg(0x01, buf);

}

char *rfm69_receive(int len) {
  rfm69_spi_setup();
  uint8_t buf;

  // Packet length
  rfm69_write_reg(0x38, len);

  // RX mode
  buf = rfm69_read_reg(0x01); // Mode Register
  buf &= 0b11100011; // keep other register variables
  buf += 0b00010000; // Mode -> RX
  rfm69_write_reg(0x01, buf);

  while((rfm69_read_reg(0x27) & 0b10000000) == 0); // RSSI sampling starts (RX mode ready)

  // read data on "on-the-fly"
  int i = 0;
  uint8_t *payload = calloc(len, sizeof(uint8_t));
  while (i < len) {
    buf = rfm69_read_reg(0x28);
    if ( (buf & 0b00000100) == 0b00000100) { // Payload Ready; nothing more to receive
      // Standby mode
      buf = rfm69_read_reg(0x01); // Mode Register
      buf &= 0b11100011; // keep other register variables
      buf += 0b00000100; // Mode -> Standby
      rfm69_write_reg(0x01, buf);
    } else if ( (buf & 0b01000000) == 0b01000000 ) { // FIFO not empty
      payload[i] = rfm69_read_reg(0x00); // FIFO Register
      i++;
    }
  }

  return payload;
}
