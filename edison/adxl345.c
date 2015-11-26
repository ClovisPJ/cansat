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

#include "adxl345.h"

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

float* adxl345_getacceleration() {
    for(int i = 0; i < 3; i++) {
        m_accel[i] = m_rawaccel[i] * m_offsets[i];
    }
    return &m_accel[0];
}

int16_t* adxl345_getrawvalues() {
    return &m_rawaccel[0];
}

uint8_t adxl345_getscale() {

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
