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
#include <stdio.h>

#include "bmpx8x.h"

int main(int argc, char **argv) {

    uint32_t pressure = 0;
    float temperature = 0;
    float altitude = 0;
    uint32_t sealevel = 0;

    // Instantiate a BMPX8X sensor on I2C
    bmpx8x_init(1, 0x77, 3);

    // Print the pressure, altitude, sea level, and
    // temperature values every 0.1 seconds
    while (1) {
        pressure = bmpx8x_getpressure ();
        temperature = bmpx8x_gettemperature ();
        altitude = bmpx8x_getaltitude (101325);
        sealevel = bmpx8x_getsealevelpressure (0);

        printf("pressure value = %d, altitude value = %f, sealevel value = %d, temperature = %f\n", pressure, altitude, sealevel, temperature);
        usleep (100000);
    }

    return EXIT_SUCCESS;
}

