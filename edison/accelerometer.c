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

#include <unistd.h>

#include "adxl345.h"

int main (int argc, char **argv) {
    int16_t *raw;
    float *acc;

    // Note: Sensor only works at 3.3V on the Intel Edison with Arduino breakout
    adxl345_init();

    while(1) {
        adxl345_update(); // Update the data
        //raw = adxl345_getrawvalues(); // Read raw sensor data
        acc = adxl345_getacceleration(); // Read acceleration (g)
        // +-1%
        fprintf(stdout, "Current scale: 0x%2xg\n", adxl345_getscale());
        //fprintf(stdout, "Raw: %6d %6d %6d\n", raw[0], raw[1], raw[2]);
        fprintf(stdout, "AccX: %5.2f g\n", acc[0]); 
        fprintf(stdout, "AccY: %5.2f g\n", acc[1]);
        fprintf(stdout, "AccZ: %5.2f g\n", acc[2]);
//        sleep(1);
    }
    return 0;
}
