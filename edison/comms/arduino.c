#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <SPI.h>

#include "arduino.h"

void rfm69_spi_setup() {
  SPI.begin();
  SPI.beginTransaction(SPISettings(10000, MSBFIRST, SPI_MODE0)); //Settings
  SPI.endTransaction(); //this won't change settings
  digitalWrite(CS, HIGH);

  Serial.begin(9600);
}
uint8_t rfm69_read_reg(uint8_t addr) {
  uint16_t in;
  in = (uint16_t)(addr << 8);
  uint16_t out;
  digitalWrite(CS, LOW);
  out = SPI.transfer16(in);
  digitalWrite(CS, HIGH);
  return (uint8_t)out; //last 8 bits
}

void rfm69_write_reg(uint8_t addr, uint8_t val) {
  uint16_t in = (uint16_t)( (0b10000000 | addr) << 8); //begin 1 for write
  in += (uint16_t)val;
  digitalWrite(CS, LOW);
  SPI.transfer16(in);
  digitalWrite(CS, HIGH);
}

uint8_t *rfm69_read_fifo(uint8_t len) {
  uint8_t *buf = (uint8_t*)calloc(len+2, sizeof(uint8_t));
  buf[0] = 0x00; // FIFO Register (though 0 already)
  digitalWrite(CS, LOW);
  SPI.transfer(buf, len+2);
  digitalWrite(CS, HIGH);
  // buf[1] is address byte
  return buf+2;
}

void rfm69_write_fifo(uint8_t *data, uint8_t len) {
  uint8_t *buf = (uint8_t*)calloc(len+2, sizeof(uint8_t));
  buf[0] = 0x00 | 0b10000000; // FIFO Register, begin 1 for write
  buf[1] = 0; // Address Byte
  memcpy(buf+2, data, len);
  digitalWrite(CS, LOW);
  SPI.transfer(buf, len+2);
  digitalWrite(CS, HIGH);
}

void settings() {
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

  // Packet length -> 4
  rfm69_write_reg(0x38, 4);

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

void send() {
  rfm69_spi_setup();
  uint8_t buf;

  // Standby mode
  buf = rfm69_read_reg(0x01); // Mode Register
  buf &= 0b11100011; // keep other register variables
  buf += 0b00000100; // Mode -> Standby
  rfm69_write_reg(0x01, buf);

  // wait until Ready
  while ((rfm69_read_reg(0x27) & 0b10000000) == 0); // until crystal oscillator is running

  // Write data to FIFO register (transmission data)
  uint8_t data[3];
  data[0] = 15;
  data[1] = 23;
  data[2] = 47;
  rfm69_write_fifo(data, 3);

  // TX mode - sends data
  buf = rfm69_read_reg(0x01); // Mode Register
  buf &= 0b11100011; // keep other register variables
  buf += 0b00001100; // Mode -> TX
  rfm69_write_reg(0x01, buf);

  buf = rfm69_read_reg(0x01); // Mode Register
  // wait until Ready
  while ((rfm69_read_reg(0x28) & 0b00001000) == 0); // until packet sent

  // Standby mode
  buf = rfm69_read_reg(0x01); // Mode Register
  buf &= 0b11100011; // keep other register variables
  buf += 0b00000100; // Mode -> Standby
  rfm69_write_reg(0x01, buf);

}

void receive() {
  rfm69_spi_setup();
  uint8_t buf;

  // RX mode
  buf = rfm69_read_reg(0x01); // Mode Register
  buf &= 0b11100011; // keep other register variables
  buf += 0b00010000; // Mode -> RX
  rfm69_write_reg(0x01, buf);

  while((rfm69_read_reg(0x27) & 0b10000000) == 0); // RSSI sampling starts (RX mode ready)

  // check for received packet
/*  while((rfm69_read_reg(0x28) & 0b00000100) == 0b0) { // IRQ Flags 2 Register; PayloadReady bit
    if ((rfm69_read_reg(0x27) & 0b00000001) == 0b1) Serial.println("Match!");
    if ((rfm69_read_reg(0x01) & 0b00011100) == 0b10000) Serial.println("RX mode");
  }*/
  while((rfm69_read_reg(0x28) & 0b00000100) == 0); // IRQ Flags 2 Register; PayloadReady bit

  // Standby mode
  buf = rfm69_read_reg(0x01); // Mode Register
  buf &= 0b11100011; // keep other register variables
  buf += 0b00000100; // Mode -> Standby
  rfm69_write_reg(0x01, buf);

  while((rfm69_read_reg(0x27) & 0b10000000) == 0); // Data can be read

  // read data from FIFO register
  uint8_t *data;
  data = rfm69_read_fifo(3);
  Serial.println(data[0]);
  Serial.println(data[1]);
  Serial.println(data[2]);

}
