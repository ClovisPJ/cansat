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

/* HTU21D Commands */
#define HTU21D_READ_TEMP_HOLD     0xE3
#define HTU21D_READ_HUMIDITY_HOLD 0xE5
#define HTU21D_WRITE_USER_REG     0xE6
#define HTU21D_READ_USER_REG      0xE7
#define HTU21D_SOFT_RESET         0xFE

/* User Register Bit Definition */
#define HTU21D_DISABLE_OTP        0x02
#define HTU21D_HEATER_ENABLE      0x04
#define HTU21D_END_OF_BATTERY     0x40
#define HTU21D_RESO_RH12_T14      0x00
#define HTU21D_RESO_RH8_T12       0x01
#define HTU21D_RESO_RH10_T13      0x80
#define HTU21D_RESO_RH11_T11      0x81

#define true 1
#define false 0

/**
 * @brief HTU21D Humidity Sensor library
 * @defgroup htu21d libupm-htu21d
 * @ingroup seeed adafruit sparkfun i2c temp
 */

/**
 * @library htu21d
 * @sensor htu21d
 * @comname HTU21D Temperature & Humidity Sensor
 * @type temp
 * @man seeed adafruit sparkfun
 * @web http://www.meas-spec.com/downloads/HTU21D.pdf
 * @con i2c
 *
 * @brief API for the HTU21D Temperature & Humidity Sensor
 *
 * HTU21D by Measurement Specialties is a digital humidity sensor with
 * temperature output.
 * RH reports between 0 and 100%, and the temperature range is
 * -40 to +125 degC. Note: getCompRH is the preferred
 * function below (passing true to cause a measurement cycle). If
 * actual values used for the compensated ready are necessary, use
 * the getHumidity(false) and getTemperature(false) functions following
 * the getCompRH call.
 * Also note the sensor should not perform more than a couple of
 * measurements per second to limit the heating of the sensor.
 *
 * @image html htu21d.jpeg
 * @snippet htu21d.cxx Interesting
 */
/**
 * Instantiates an HTU21D object
 *
 * @param bus Number of the used bus
 * @param devAddr Address of the used I2C device
 * @param mode HTU21D oversampling
 */
int htu21d_init (int bus, int devAddr);

/**
 * Initiates a temperature/pressure mesasurement and waits for the function
 * to complete. The humidity and temperature registers can be read
 * after this call.
 */
int htu21d_sampledata();

/**
 * Gets the current measured humidity [RH]
 */
float htu21d_gethumidity(int bSampleData);

/**
 * Gets the humidity cell temperature [degC]
 */
float htu21d_gettemperature(int bSampleData);

/**
 * Using the current humidity and temperature, the function
 * calculates the compensated RH using the equation from
 * the datasheet.
 */
float htu21d_getcompRH(int bSampleData);

/**
 * Sets the heater state. The heater is used to test
 * the sensor functionality since the temperature should increase
 * 0.5 to 1.5 degC, and the humidity should decrease. The
 * testSensor() function below uses the heater.
 *
 * @param bEnable Sets to non-zero to turn the heater on 
 */
 int htu21d_setheater(int bEnable);

/**
 * Performs a soft reset of the MPL3115A2 device to ensure
 * it is in a known state. This function can be used to reset
 * the min/max temperature and pressure values.
 */
void htu21d_resetsensor();

/**
 * Tests the device and verifies it
 * is operating correctly.
 *
 */
int htu21d_testsensor();

/**
 * Writes to a one-byte register
 *
 * @param reg Address of the register
 * @param value Byte to be written
 */
int htu21d_writereg (uint8_t reg, uint8_t value);

/**
 * Reads a two-byte register
 *
 * @param reg Address of the register
 */
uint16_t htu21d_readreg_16 (int reg);

/**
 * Reads a one-byte register
 *
 * @param reg Address of the register
 */
uint8_t htu21d_readreg_8 (int reg);

/**
 * Converts the temperature register to degC * 1000
 */
int32_t htu21d_converttemp(int32_t regval);

/**
 * Converts the RH register to %RH * 1000
 */
int32_t htu21d_convertRH(int32_t regval);

char m_name[6];

int m_controlAddr;
int m_bus;
mraa_i2c_context i2c;

int32_t m_temperature;
int32_t m_humidity;

