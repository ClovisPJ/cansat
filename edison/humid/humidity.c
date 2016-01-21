/*
 * Author: William Penner <william.penner@intel.com>
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

#include "htu21d.h"

int main(int argc, char **argv) {

    float humidity    = 0.0;
    float temperature = 0.0;
    float compRH      = 0.0;

    htu21d_init(1, HTU21D_I2C_ADDRESS);

    htu21d_testsensor();

    while (1) {
        compRH      = htu21d_getcompRH(1);
        humidity    = htu21d_gethumidity(0);
        temperature = htu21d_gettemperature(0);

        printf("humidity value = %f\n", humidity); //+- 2%
        printf("temperature value = %f\n", temperature); //SD: 0.1 degC
        printf("compensated RH value = %f\n", compRH);
        usleep (500000);
    }

    return 0;
}

