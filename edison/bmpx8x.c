/*
 * Author: Yevgeniy Kiveisha <yevgeniy.kiveisha@intel.com>
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

#include <iostream>
#include <string>
#include <stdexcept>
#include <unistd.h>
#include <stdlib.h>
#include <mraa/i2c.h>
#include <math.h>

#define ADDR               0x77 // device address

// registers address
#define BMP085_ULTRALOWPOWER 0
#define BMP085_STANDARD      1
#define BMP085_HIGHRES       2
#define BMP085_ULTRAHIGHRES  3
#define BMP085_CAL_AC1           0xAA  // R   Calibration data (16 bits)
#define BMP085_CAL_AC2           0xAC  // R   Calibration data (16 bits)
#define BMP085_CAL_AC3           0xAE  // R   Calibration data (16 bits)
#define BMP085_CAL_AC4           0xB0  // R   Calibration data (16 bits)
#define BMP085_CAL_AC5           0xB2  // R   Calibration data (16 bits)
#define BMP085_CAL_AC6           0xB4  // R   Calibration data (16 bits)
#define BMP085_CAL_B1            0xB6  // R   Calibration data (16 bits)
#define BMP085_CAL_B2            0xB8  // R   Calibration data (16 bits)
#define BMP085_CAL_MB            0xBA  // R   Calibration data (16 bits)
#define BMP085_CAL_MC            0xBC  // R   Calibration data (16 bits)
#define BMP085_CAL_MD            0xBE  // R   Calibration data (16 bits)

#define BMP085_CONTROL           0xF4
#define BMP085_TEMPDATA          0xF6
#define BMP085_PRESSUREDATA      0xF6
#define BMP085_READTEMPCMD       0x2E
#define BMP085_READPRESSURECMD   0x34

#define HIGH               1
#define LOW                0

char[] m_name;

int m_controlAddr;
int m_bus;
mraa_i2c_context i2c;

uint8_t oversampling;
int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
uint16_t ac4, ac5, ac6;

int bmpx8x_init (int bus, int devAddr, uint8_t mode);
int32_t getpressure ();
int32_t getpressureraw ();
int16_t gettemperatureraw ();
float gettemperature ();
int32_t getsealevelpressure(float altitudemeters = 0);
float getaltitude (float sealevelpressure = 101325);
int32_t computeB5 (int32_t UT);
uint16_t i2creadreg_16 (int reg);
int i2cwritereg (uint8_t reg, uint8_t value);
uint8_t i2creadreg_8 (int reg);

int bmpx8x_init (int bus, int devAddr, uint8_t mode) {
 
    m_name = "BMPX8X";
 
    mraa_init();
    i2c = mraa_i2c_init(bus);

    if (i2c == NULL) {
        printf("i2c context init failed");
        return EXIT_FAILURE;
    }

    if (mraa_i2c_address(i2c, devAddr) != EXIT_SUCCESS ){
        printf("i2c.address() failed");
        return EXIT_FAILURE;
    }

    if (bmpx8x_readreg_8(0xD0) != 0x55)  {
        printf("Invalid chip ID");
        return EXIT_FAILURE;
    }

    if (mode > BMP085_ULTRAHIGHRES) {
        mode = BMP085_ULTRAHIGHRES;
    }
    oversampling = mode;

    /* read calibration data */
    ac1 = bmpx8x_readreg_16 (BMP085_CAL_AC1);
    ac2 = bmpx8x_readreg_16 (BMP085_CAL_AC2);
    ac3 = bmpx8x_readreg_16 (BMP085_CAL_AC3);
    ac4 = bmpx8x_readreg_16 (BMP085_CAL_AC4);
    ac5 = bmpx8x_readreg_16 (BMP085_CAL_AC5);
    ac6 = bmpx8x_readreg_16 (BMP085_CAL_AC6);

    b1 = bmpx8x_readreg_16 (BMP085_CAL_B1);
    b2 = bmpx8x_readreg_16 (BMP085_CAL_B2);

    mb = bmpx8x_readreg_16 (BMP085_CAL_MB);
    mc = bmpx8x_readreg_16 (BMP085_CAL_MC);
    md = bmpx8x_readreg_16 (BMP085_CAL_MD);
}

int32_t bmpx8x_getpressure () {
    int32_t UT, UP, B3, B5, B6, X1, X2, X3, p;
    uint32_t B4, B7;

    UT = gettemperatureraw();
    UP = getpressureraw();
    B5 = computeB5(UT);

    // do pressure calcs
    B6 = B5 - 4000;
    X1 = ((int32_t)b2 * ( (B6 * B6)>>12 )) >> 11;
    X2 = ((int32_t)ac2 * B6) >> 11;
    X3 = X1 + X2;
    B3 = ((((int32_t)ac1*4 + X3) << oversampling) + 2) / 4;

    X1 = ((int32_t)ac3 * B6) >> 13;
    X2 = ((int32_t)b1 * ((B6 * B6) >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    B4 = ((uint32_t)ac4 * (uint32_t)(X3 + 32768)) >> 15;
    B7 = ((uint32_t)UP - B3) * (uint32_t)( 50000UL >> oversampling );

    if (B7 < 0x80000000) {
        p = (B7 * 2) / B4;
    } else {
        p = (B7 / B4) * 2;
    }
    X1 = (p >> 8) * (p >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * p) >> 16;

    p = p + ((X1 + X2 + (int32_t)3791)>>4);

    return p;
}

int32_t bmpx8x_getpressureraw () {
    uint32_t raw;

    bmpx8x_writereg (BMP085_CONTROL, BMP085_READPRESSURECMD + (oversampling << 6));

    if (oversampling == BMP085_ULTRALOWPOWER) {
        usleep(5000);
    } else if (oversampling == BMP085_STANDARD) {
        usleep(8000);
    } else if (oversampling == BMP085_HIGHRES) {
        usleep(14000);
    } else {
        usleep(26000);
    }

    raw = bmpx8x_readreg_16 (BMP085_PRESSUREDATA);

    raw <<= 8;
    raw |= bmpx8x_readreg_8 (BMP085_PRESSUREDATA + 2);
    raw >>= (8 - oversampling);

    return raw;
}

int16_t bmpx8x_gettemperatureraw () {
    bmpx8x_writereg (BMP085_CONTROL, BMP085_READTEMPCMD);
    usleep(5000);
    return bmpx8x_readreg_16 (BMP085_TEMPDATA);
}

float bmpx8x_gettemperature () {
    int32_t UT, B5;     // following ds convention
    float temp;

    UT = bmpx8x_gettemperatureraw ();

    B5 = computeB5 (UT);
    temp = (B5 + 8) >> 4;
    temp /= 10;

    return temp;
}

int32_t bmpx8x_getsealevelpressure (float altitudemeters) {
    float pressure = bmpx8x_getpressure ();
    return (int32_t)(pressure / pow(1.0-altitudemeters/44330, 5.255));
}

float bmpx8x_getaltitude (float sealevelpressure) {
    float altitude;

    float pressure = bmpx8x_getPressure ();

    altitude = 44330 * (1.0 - pow(pressure /sealevelpressure,0.1903));

    return altitude;
}

int32_t bmpx8x_computeB5 (int32_t UT) {
    int32_t X1 = (UT - (int32_t)ac6) * ((int32_t)ac5) >> 15;
    int32_t X2 = ((int32_t)mc << 11) / (X1+(int32_t)md);

    return X1 + X2;
}

int bmpx8x_writereg (uint8_t reg, uint8_t value) {

    uint8_t data[2] = { reg, value };

    error = mraa_i2c_address(i2c,m_controlAddr);
    error = mraa_i2c_write(i2c, data, 2);

    return error;
}

uint16_t bmpx8x_readreg_16 (int reg) {
    uint16_t data;

    mraa_i2c_address(i2c,m_controlAddr);
    mraa_i2c_write_byte_data(i2c, reg, m_controlAddr);

    mraa_i2c_read_bytes_data(i2c, m_controlAddr, (uint8_t *)&data, 0x2);

    uint8_t high = (data & 0xFF00) >> 8;
    data = (data << 8) & 0xFF00;
    data |= high;

    return data;
}

uint8_t bmpx8x_readreg_8 (int reg) {
    uint8_t data;

    mraa_i2c_address(i2c,m_controlAddr);
    mraa_i2c_write_byte_data(i2c, reg, m_controlAddr);

    mraa_i2c_read_bytes_data(i2c, m_controlAddr, &data, 0x1);

    return data;
}
