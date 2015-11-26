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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <mraa.h>

#include "htu21d.h"

int htu21d_init (int bus, int devAddr) {
    m_temperature = 0;
    m_humidity    = 0;

    strcpy(m_name, HTU21D_NAME);

    m_controlAddr = devAddr;
    m_bus = bus;

    mraa_init();
    i2c = mraa_i2c_init(bus);

    if (i2c == NULL) {
        printf("i2c context init failed");
        return EXIT_FAILURE;
    }

    if ( mraa_i2c_address(i2c, HTU21D_I2C_ADDRESS) != EXIT_SUCCESS ){
        printf("i2c.address() failed");
        return EXIT_FAILURE;
    }

    htu21d_resetsensor();
}

void htu21d_resetsensor() {
    uint8_t data;
    mraa_i2c_address(i2c, m_controlAddr);
    mraa_i2c_write(i2c, &data, 1);
    usleep(20000);
}

/*
 * Convert register value to degC * 1000
 */
int32_t htu21d_converttemp(int32_t regval) {
    return ((21965 * (regval & 0xFFFC)) >> 13) - 46850;
}

/*
 * Convert register value to %RH * 1000
 */
int32_t htu21d_convertRH(int regval) {
    return ((15625 * (regval & 0xFFFC)) >> 13) - 6000;
}

int htu21d_sampledata(void) {
    uint32_t itemp;

    itemp = htu21d_readreg_16(HTU21D_READ_TEMP_HOLD);
    m_temperature = htu21d_converttemp(itemp);

    itemp = htu21d_readreg_16(HTU21D_READ_HUMIDITY_HOLD);
    m_humidity = htu21d_convertRH(itemp);

    return 0;
}

float htu21d_gettemperature(int bSampleData) {
    if (bSampleData) {
        htu21d_sampledata();
    }
    return (float)m_temperature / 1000;
}

float htu21d_gethumidity(int bSampleData) {
    if (bSampleData) {
        htu21d_sampledata();
    }
    return (float)m_humidity / 1000;
}

/*
 * Use the compensation equation from the datasheet to correct the
 * current reading
 * RHcomp = RHactualT + (25 - Tactual) * CoeffTemp
 * RHcomp is in units of %RH * 1000
 */
float htu21d_getcompRH(int bSampleData) {
	if (bSampleData) {
	  htu21d_sampledata();
	}
	return (float)(m_humidity + (25000 - m_temperature) * 3 / 20) / 1000;
}

int htu21d_setheater(int bEnable) {
	uint8_t userreg;

	userreg = htu21d_readreg_8(HTU21D_READ_USER_REG);
	if (bEnable)
		userreg |= HTU21D_HEATER_ENABLE;
	else
		userreg &= ~HTU21D_HEATER_ENABLE;
	if (htu21d_writereg(HTU21D_WRITE_USER_REG, userreg) < 0)
		return -1;

	return 0;
}

/*
 * Test function: when reading the HTU21D many times rapidly should
 * result in a temperature increase.  This test will verify that the
 * value is changing from read to read
 */

int htu21d_testsensor() {
    int i;
    int iError = 0;
    float fTemp, fHum;
    float fTempMax, fTempMin;
    float fHumMax, fHumMin;
    float fHumFirst, fTempFirst;

    fprintf(stdout, "Executing Sensor Test\n" );

    fHum  = htu21d_gethumidity(true);
    fTemp = htu21d_gettemperature(false);
    fTempFirst = fTempMax = fTempMin = fTemp;
    fHumFirst  = fHumMax  = fHumMin  = fHum;

    // Turn on the heater to make a sensor change
    htu21d_setheater(true);

    // Then sample the sensor a few times
    for (i=0; i < 10; i++) {
        fHum  = htu21d_gethumidity(true);
        fTemp = htu21d_gettemperature(false);
        if (fHum  < fHumMin)  fHumMin  = fHum;
        if (fHum  > fHumMax)  fHumMax  = fHum;
        if (fTemp < fTempMin) fTempMin = fTemp;
        if (fTemp > fTempMax) fTempMax = fTemp;
        usleep(50000);
    }

    // Turn off the heater
    htu21d_setheater(false);

    // Now check the results
    if ((fTemp - fTempFirst) <= 0) {
        fprintf(stdout, "  Temperature should have increased, but didn't\n" );
        iError++;
    }
    if (fHumMin == fHumMax) {
        fprintf(stdout, "  Humidity reading was unchanged - warning\n" );
        iError++;
    }
    if (fTempMin == fTempMax) {
        fprintf(stdout, "  Temperature reading was unchanged - warning\n" );
        iError++;
    }
    if (iError == 0) {
        fprintf(stdout, "  Device appears functional\n" );
    }

    fprintf(stdout, "  Test complete\n" );

    return iError;
}

/*
 * Functions to read and write data to the i2c device
 */

int htu21d_writereg (uint8_t reg, uint8_t value) {
    int error;

    uint8_t data[2] = { reg, value };
    mraa_i2c_address(i2c, m_controlAddr);
    error = mraa_i2c_write (i2c, data, 2);
    if ( error != EXIT_SUCCESS) {
      printf("mraa_i2c_write() failed\n");
    }
    return error;
}

uint16_t htu21d_readreg_16 (int reg) {
    uint16_t data;
    mraa_i2c_address(i2c, m_controlAddr);
    data  = (uint16_t)mraa_i2c_read_byte_data(i2c, reg) << 8;
    data |= (uint16_t)mraa_i2c_read_byte_data(i2c, reg+1);
    return data;
}

uint8_t htu21d_readreg_8 (int reg) {
    mraa_i2c_address(i2c, m_controlAddr);
    return mraa_i2c_read_byte_data(i2c, reg);
}
