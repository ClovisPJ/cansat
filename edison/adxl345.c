/*
 * Author: Mihai Tudor Panu <mihai.tudor.panu@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <mraa.h>

#define READ_BUFFER_LENGTH 6

//address and id
#define ADXL345_I2C_ADDR 0x53
#define ADXL345_ID 0x00

//control registers
#define ADXL345_OFSX 0x1E
#define ADXL345_OFSY 0x1F
#define ADXL345_OFSZ 0x20
#define ADXL345_TAP_THRESH 0x1D
#define ADXL345_TAP_DUR 0x21
#define ADXL345_TAP_LATENCY 0x22
#define ADXL345_ACT_THRESH 0x24
#define ADXL345_INACT_THRESH 0x25
#define ADXL345_INACT_TIME 0x26
#define ADXL345_INACT_ACT_CTL 0x27
#define ADXL345_FALL_THRESH 0x28
#define ADXL345_FALL_TIME 0x29
#define ADXL345_TAP_AXES 0x2A
#define ADXL345_ACT_TAP_STATUS 0x2B

//interrupt registers
#define ADXL345_INT_ENABLE 0x2E
#define ADXL345_INT_MAP 0x2F
#define ADXL345_INT_SOURCE 0x30

//data registers (read only)
#define ADXL345_XOUT_L 0x32
#define ADXL345_XOUT_H 0x33
#define ADXL345_YOUT_L 0x34
#define ADXL345_YOUT_H 0x35
#define ADXL345_ZOUT_L 0x36
#define ADXL345_ZOUT_H 0x37
#define DATA_REG_SIZE 6

//data and power management
#define ADXL345_BW_RATE 0x2C
#define ADXL345_POWER_CTL 0x2D
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_FIFO_CTL 0x38
#define ADXL345_FIFO_STATUS 0x39

//useful values
#define ADXL345_POWER_ON 0x08
#define ADXL345_AUTO_SLP 0x30
#define ADXL345_STANDBY 0x00

//scales and resolution
#define ADXL345_FULL_RES 0x08
#define ADXL345_10BIT 0x00
#define ADXL345_2G 0x00
#define ADXL345_4G 0x01
#define ADXL345_8G 0x02
#define ADXL345_16G 0x03

float m_accel[3];
float m_offsets[3];
int16_t m_rawaccel[3];
uint8_t m_buffer[READ_BUFFER_LENGTH];

int adxl345_init();
float* adxl345_getAcceleration();
int16_t* adxl345_getRawValues();
uint8_t adxl345_getScale();
int adxl345_update();

mraa_i2c_context i2c;

int adxl345_init() {

    mraa_init();
    i2c = mraa_i2c_init(1);

    if (i2c == NULL) {
        printf("i2c context init failed");
        return EXIT_FAILURE;
    }

    if ( mraa_i2c_address(i2c, ADXL345_I2C_ADDR) != EXIT_SUCCESS ){
        printf("i2c.address() failed");
        return EXIT_FAILURE;
    }

    m_buffer[0] = ADXL345_POWER_CTL;
    m_buffer[1] = ADXL345_POWER_ON;
    if ( mraa_i2c_write(i2c, m_buffer, 2) != EXIT_SUCCESS){
        printf("i2c.write() control register failed");
        return EXIT_FAILURE;
    }

    if ( mraa_i2c_address(i2c, ADXL345_I2C_ADDR) != EXIT_SUCCESS ){
        printf("i2c.address() failed");
        return EXIT_FAILURE;
    }

    m_buffer[0] = ADXL345_DATA_FORMAT;
    m_buffer[1] = ADXL345_16G | ADXL345_FULL_RES;
    if( mraa_i2c_write(i2c, m_buffer, 2) != EXIT_SUCCESS){
        printf("i2c.write() mode register failed");
        return EXIT_FAILURE;
    }

    //2.5V sensitivity is 256 LSB/g = 0.00390625 g/bit
    //3.3V x and y sensitivity is 265 LSB/g = 0.003773584 g/bit, z is the same

    m_offsets[0] = 0.003773584;
    m_offsets[1] = 0.003773584;
    m_offsets[2] = 0.00390625;

    adxl345_update();
}

float* adxl345_getAcceleration() {
    for(int i = 0; i < 3; i++) {
        m_accel[i] = m_rawaccel[i] * m_offsets[i];
    }
    return &m_accel[0];
}

int16_t* adxl345_getRawValues() {
    return &m_rawaccel[0];
}

uint8_t adxl345_getScale() {

    uint8_t result;

    mraa_i2c_address(i2c, ADXL345_I2C_ADDR);

    mraa_i2c_write_byte(i2c, ADXL345_DATA_FORMAT);

    result = mraa_i2c_read_byte(i2c);

    return pow(2, (result & 0x03) + 1);
}

int adxl345_update()
{
    mraa_i2c_address(i2c, ADXL345_I2C_ADDR);
    mraa_i2c_write_byte(i2c, ADXL345_XOUT_L);

    mraa_i2c_read(i2c, m_buffer, DATA_REG_SIZE);

    // x
    m_rawaccel[0] = ((m_buffer[1] << 8 ) | m_buffer[0]);
    // y
    m_rawaccel[1] = ((m_buffer[3] << 8 ) | m_buffer[2]);
    // z
    m_rawaccel[2] = ((m_buffer[5] << 8 ) | m_buffer[4]);

    return EXIT_SUCCESS;
}
