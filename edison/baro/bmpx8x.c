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

#include <unistd.h>
#include <stdlib.h>
#include <mraa/i2c.h>
#include <math.h>
#include <string.h>

#include "bmpx8x.h"

int bmpx8x_init (int bus, int devAddr, uint8_t mode) {
 
    strcpy(bmpx8x_m_name, "BMPX8X");
    bmpx8x_m_controlAddr = devAddr;
 
    mraa_init();
    bmpx8x_i2c = mraa_i2c_init(bus);

    if (bmpx8x_i2c == NULL) {
        printf("i2c context init failed");
        return EXIT_FAILURE;
    }

    if (mraa_i2c_address(bmpx8x_i2c, devAddr) != EXIT_SUCCESS ){
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
    bmpx8x_oversampling = mode;

    /* read calibration data */
    bmpx8x_ac1 = bmpx8x_readreg_16 (BMP085_CAL_AC1);
    bmpx8x_ac2 = bmpx8x_readreg_16 (BMP085_CAL_AC2);
    bmpx8x_ac3 = bmpx8x_readreg_16 (BMP085_CAL_AC3);
    bmpx8x_ac4 = bmpx8x_readreg_16 (BMP085_CAL_AC4);
    bmpx8x_ac5 = bmpx8x_readreg_16 (BMP085_CAL_AC5);
    bmpx8x_ac6 = bmpx8x_readreg_16 (BMP085_CAL_AC6);

    bmpx8x_b1 = bmpx8x_readreg_16 (BMP085_CAL_B1);
    bmpx8x_b2 = bmpx8x_readreg_16 (BMP085_CAL_B2);

    bmpx8x_mb = bmpx8x_readreg_16 (BMP085_CAL_MB);
    bmpx8x_mc = bmpx8x_readreg_16 (BMP085_CAL_MC);
    bmpx8x_md = bmpx8x_readreg_16 (BMP085_CAL_MD);

    return EXIT_SUCCESS;
}

int32_t bmpx8x_getpressure () {
    int32_t UT, UP, B3, B5, B6, X1, X2, X3, p;
    uint32_t B4, B7;

    UT = bmpx8x_gettemperatureraw();
    UP = bmpx8x_getpressureraw();
    B5 = bmpx8x_computeB5(UT);

    // do pressure calcs
    B6 = B5 - 4000;
    X1 = ((int32_t)bmpx8x_b2 * ( (B6 * B6)>>12 )) >> 11;
    X2 = ((int32_t)bmpx8x_ac2 * B6) >> 11;
    X3 = X1 + X2;
    B3 = ((((int32_t)bmpx8x_ac1*4 + X3) << bmpx8x_oversampling) + 2) / 4;

    X1 = ((int32_t)bmpx8x_ac3 * B6) >> 13;
    X2 = ((int32_t)bmpx8x_b1 * ((B6 * B6) >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    B4 = ((uint32_t)bmpx8x_ac4 * (uint32_t)(X3 + 32768)) >> 15;
    B7 = ((uint32_t)UP - B3) * (uint32_t)( 50000UL >> bmpx8x_oversampling );

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

    bmpx8x_writereg (BMP085_CONTROL, BMP085_READPRESSURECMD + (bmpx8x_oversampling << 6));

    if (bmpx8x_oversampling == BMP085_ULTRALOWPOWER) {
        usleep(5000);
    } else if (bmpx8x_oversampling == BMP085_STANDARD) {
        usleep(8000);
    } else if (bmpx8x_oversampling == BMP085_HIGHRES) {
        usleep(14000);
    } else {
        usleep(26000);
    }

    raw = bmpx8x_readreg_16 (BMP085_PRESSUREDATA);

    raw <<= 8;
    raw |= bmpx8x_readreg_8 (BMP085_PRESSUREDATA + 2);
    raw >>= (8 - bmpx8x_oversampling);

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

    B5 = bmpx8x_computeB5 (UT);
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

    float pressure = bmpx8x_getpressure ();

    altitude = 44330 * (1.0 - pow(pressure /sealevelpressure,0.1903));

    return altitude;
}

int32_t bmpx8x_computeB5 (int32_t UT) {
    int32_t X1 = (UT - (int32_t)bmpx8x_ac6) * ((int32_t)bmpx8x_ac5) >> 15;
    int32_t X2 = ((int32_t)bmpx8x_mc << 11) / (X1+(int32_t)bmpx8x_md);

    return X1 + X2;
}

int bmpx8x_writereg (uint8_t reg, uint8_t value) {
    int error;

    uint8_t data[2] = { reg, value };

    error = mraa_i2c_address(bmpx8x_i2c, bmpx8x_m_controlAddr);
    error = mraa_i2c_write(bmpx8x_i2c, data, 2);

    return error;
}

uint16_t bmpx8x_readreg_16 (int reg) {
    uint16_t data;

    mraa_i2c_address(bmpx8x_i2c, bmpx8x_m_controlAddr);
    mraa_i2c_write_byte(bmpx8x_i2c, reg);

    mraa_i2c_address(bmpx8x_i2c, bmpx8x_m_controlAddr);
    mraa_i2c_read(bmpx8x_i2c, (uint8_t *)&data, 0x2);

    uint8_t high = (data & 0xFF00) >> 8;
    data = (data << 8) & 0xFF00;
    data |= high;

    return data;
}

uint8_t bmpx8x_readreg_8 (int reg) {
    uint8_t data;

    mraa_i2c_address(bmpx8x_i2c, bmpx8x_m_controlAddr);
    mraa_i2c_write_byte(bmpx8x_i2c, reg);

    mraa_i2c_address(bmpx8x_i2c, bmpx8x_m_controlAddr);
    mraa_i2c_read(bmpx8x_i2c, &data, 0x1);

    return data;
}
