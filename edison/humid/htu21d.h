/*
 * Author: William Penner <william.penner@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <mraa.h>
#include <math.h>

#define HTU21D_NAME "htu21d"
#define HTU21D_I2C_ADDRESS 0x40

#define HTU21D_READ_TEMP_HOLD     0xE3
#define HTU21D_READ_HUMIDITY_HOLD 0xE5
#define HTU21D_WRITE_USER_REG     0xE6
#define HTU21D_READ_USER_REG      0xE7
#define HTU21D_SOFT_RESET         0xFE

#define HTU21D_DISABLE_OTP        0x02
#define HTU21D_HEATER_ENABLE      0x04
#define HTU21D_END_OF_BATTERY     0x40
#define HTU21D_RESO_RH12_T14      0x00
#define HTU21D_RESO_RH8_T12       0x01
#define HTU21D_RESO_RH10_T13      0x80
#define HTU21D_RESO_RH11_T11      0x81

int htu21d_init (int bus, int devAddr);

int htu21d_sampledata();

float htu21d_gethumidity(int bSampleData);

float htu21d_gettemperature(int bSampleData);

float htu21d_getcompRH(int bSampleData);

 int htu21d_setheater(int bEnable);

void htu21d_resetsensor();

int htu21d_testsensor();

int htu21d_writereg (uint8_t reg, uint8_t value);

uint16_t htu21d_readreg_16 (int reg);

uint8_t htu21d_readreg_8 (int reg);

int32_t htu21d_converttemp(int32_t regval);

int32_t htu21d_convertRH(int32_t regval);

char htu21d_m_name[6];

int htu21d_m_controlAddr;
int htu21d_m_bus;
mraa_i2c_context htu21d_i2c;

int32_t htu21d_m_temperature;
int32_t htu21d_m_humidity;

