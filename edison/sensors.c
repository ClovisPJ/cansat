#include <unistd.h>
#include <stdio.h>

#include "adxl345.h"
#include "bmpx8x.h"
#include "htu21d.h"

int main (int argc, char **argv) {

    int16_t *raw;
    float *acc;
    // Note: Sensor only works at 3.3V on the Intel Edison with Arduino breakout
    adxl345_init();

    uint32_t pressure = 0;
    float temperature = 0;
    float altitude = 0;
    uint32_t sealevel = 0;
    // Instantiate a BMPX8X sensor on I2C
    bmpx8x_init(1, 0x77, 3);

    float humidity    = 0.0;
    float temperature = 0.0;
    float compRH      = 0.0;
    htu21d_init(1, HTU21D_I2C_ADDRESS);
    htu21d_testsensor();

    while(1) {

        adxl345_update(); // Update the data
        //raw = adxl345_getrawvalues(); // Read raw sensor data
        acc = adxl345_getacceleration(); // Read acceleration (g) (+-1%)
        fprintf(stdout, "Current scale: 0x%2xg\n", adxl345_getscale());
        //fprintf(stdout, "Raw: %6d %6d %6d\n", raw[0], raw[1], raw[2]);
        fprintf(stdout, "AccX: %5.2f g\n", acc[0]); 
        fprintf(stdout, "AccY: %5.2f g\n", acc[1]);
        fprintf(stdout, "AccZ: %5.2f g\n", acc[2]);

        pressure = bmpx8x_getpressure (); //SD: 4 Pa
        temperature = bmpx8x_gettemperature (); // SD: 5/30 degC
        altitude = bmpx8x_getaltitude (101325); // SD: 1/3 degC
        sealevel = bmpx8x_getsealevelpressure (0); // SD: 4 Pa
        printf("pressure value = %d, altitude value = %f, sealevel value = %d, temperature = %f\n", pressure, altitude, sealevel, temperature);

        compRH      = htu21d_getcompRH(true);
        humidity    = htu21d_gethumidity(false);
        temperature = htu21d_gettemperature(false);
        printf("humidity value = %f\n", humidity); //+- 2%
        printf("temperature value = %f\n", temperature); //SD: 0.1 degC
        printf("compensated RH value = %f\n", compRH);
        usleep (500000);

    }

    return EXIT_SUCCESS;
}
