#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <gsl/gsl_matrix.h>

#include "accel/adxl345.h"
#include "baro/bmpx8x.h"
#include "humid/htu21d.h"
#include "adc/ads1115.h"
#include "servo/servo.h"
#include "gps/gps.h"
#include "exttm.h"
#include "comms/comms.h"
#include "comms/edison/rfm69.h"
#include "kalman/kalmanfilter.h"

int main (int argc, char **argv) {

    struct comms_Packet pck;
//    struct comms_Control ctrl;

    adxl345_init();
    bmpx8x_init(1, BMP085_ADDR, 3);
    htu21d_init(1, HTU21D_I2C_ADDRESS);
    htu21d_testsensor();
    servo_init();
    gps_init();

    comms_codelen = 4;
    rfm69_settings();

    kalmanfilter_setup();
    float *matricise = calloc(3, sizeof(float));
    gsl_matrix *location;
    gsl_matrix *temp_location;
    gsl_matrix *velocity;
    gsl_matrix *acceleration;
    int first = 1;
    float timestep;
    struct exttm temp_t;

    float destination[2];
    // Long&Lat for steering, need choosing pre-flight

/*    mraa_gpio_context irq_gpio;
    irq_gpio = mraa_gpio_init(); //pin?
    mraa_gpio_dir(irq_gpio, MRAA_GPIO_IN);
*/
    while(1) {

      adxl345_update();
      float *acc = adxl345_getacceleration(); // Read acceleration (g) (+-1%)
      memcpy(pck.acc, acc, 3*sizeof(float));
      pck.scale = adxl345_getscale();

      pck.pressure = bmpx8x_getpressure (); //SD: 4 Pa
      pck.temperature1 = bmpx8x_gettemperature (); // SD: 5/30 degC
      pck.altitude1 = bmpx8x_getaltitude (101325); // SD: 1/3 degC
      pck.sealevel = bmpx8x_getsealevelpressure (0); // SD: 4 Pa

      pck.compRH      = htu21d_getcompRH(1);
      pck.humidity    = htu21d_gethumidity(0);
      pck.temperature2 = htu21d_gettemperature(0);

      ads1115_init(0);
      pck.gas1 = ads1115_read();
      ads1115_init(1);
      pck.gas2 = ads1115_read();

      pck.servo_ang = servo_ang;

      if (gps_get_nmea("$GPRMC") == 0) {
        gps_get_nmea("$GPGGA");

        memcpy(matricise, gps_location, 2*sizeof(float));
        if (gps_satelites >= 4) {
          matricise[2] = pck.altitude2; // GPS altitude
        } else {
          matricise[2] = pck.altitude1; // barometer altitude
        }
        location = kalmanfilter_matrix(matricise);

        if (first == 0) { // this is so we have a difference for velocity
          memcpy(matricise, pck.acc, 3*sizeof(float));
          acc[0] *= pck.scale*9.8;
          acc[1] *= pck.scale*9.8;
          acc[2] *= pck.scale*9.8;
          acceleration = kalmanfilter_matrix(matricise);

          gsl_matrix_memcpy(velocity, location);
          gsl_matrix_sub(velocity, temp_location);

          timestep = (gps_time.tm_msec*0.001+gps_time.tm_sec+gps_time.tm_min*60) - (temp_t.tm_msec*0.001+temp_t.tm_sec+temp_t.tm_min*60); // only goes into minutes
          if (timestep <= 0) timestep = 0.001; // just in case sampling is too fast

          kalmanfilter_step(location, velocity, acceleration, timestep, gps_hdop, 1); // error values are incorrect
        } else {
          first = 0;
        }
        temp_t = gps_time;
        gsl_matrix_memcpy(temp_location, location);

        memcpy(pck.location, gps_location, 2*sizeof(float));
        pck.time = gps_time;

        pck.speed = gps_speed;
        pck.course = gps_course;
        pck.fix_quality = gps_fix_quality;
        pck.satelites = gps_satelites;
        pck.hdop = gps_hdop;
        pck.altitude2 = gps_altitude;
        pck.ellipsoid_seperation = gps_ellipsoid_seperation;

        float intended[2];
        intended[0] = destination[0] - gps_location[0];
        intended[1] = destination[1] - gps_location[1];
        
        float ang;
        ang = atan2(intended[0], intended[1]); // using x/y as this gives ang from N instead of x-axis
        if (ang < 0) ang += 360;
        ang -= gps_course;

        if ((ang >= 90) && (ang <= 180)) { // perpendicular or more to destination on right
          servo_changeang(180); // pull right side of servo down, steer right
        } else if ((ang <= 270) && (ang > 180)) { // perpendicular or more to destination on left 
          servo_changeang(-180); // pull left side of servo down, steer left
        } else if (ang < 90) {
          servo_changeang(ang); // pull right side of servo down, linear scale to missed angle, steer right
        } else if (ang > 270) {
          servo_changeang(360-ang); // pull left side of servo down, linear scale to missed angle, steer left 
        }

      } else printf("No fix\n");
      FILE *f;
      f = fopen("data.csv","w");
      fprintf(f, "%f,", pck.acc[0]);
      fprintf(f, "%f,", pck.acc[1]);
      fprintf(f, "%f,", pck.acc[2]);
      fprintf(f, "%d,", pck.scale);
      fprintf(f, "%d,", pck.pressure);
      fprintf(f, "%f,", pck.temperature1);
      fprintf(f, "%f,", pck.altitude1);
      fprintf(f, "%d,", pck.sealevel);
      fprintf(f, "%f,", pck.humidity);
      fprintf(f, "%f,", pck.temperature2);
      fprintf(f, "%f,", pck.compRH);
      fprintf(f, "%f,", pck.gas1);
      fprintf(f, "%f,", pck.gas2);
      fprintf(f, "%d,", pck.servo_ang);
      struct tm mkify = (struct tm){pck.time.tm_sec, pck.time.tm_min, pck.time.tm_hour, pck.time.tm_mday, pck.time.tm_mon, pck.time.tm_year, pck.time.tm_wday, pck.time.tm_yday};
      fprintf(f, "%d,", (uint32_t)(mktime(&mkify)));
      fprintf(f, "%f,", pck.location[0]);
      fprintf(f, "%f,", pck.location[1]);
      fprintf(f, "%f,", pck.speed);
      fprintf(f, "%f,", pck.course);
      fprintf(f, "%d,", pck.fix_quality);
      fprintf(f, "%d,", pck.satelites);
      fprintf(f, "%f,", pck.hdop);
      fprintf(f, "%f,", pck.altitude2);
      fprintf(f, "%f\n", pck.ellipsoid_seperation);
      fclose(f);
  
      printf("AccX: %5.2f g\n", pck.acc[0]); 
      printf("AccY: %5.2f g\n", pck.acc[1]);
      printf("AccZ: %5.2f g\n", pck.acc[2]);
      printf("Current scale: 0x%2xg\n", pck.scale);
      printf("pressure value = %d", pck.pressure);
      printf("altitude value = %f", pck.altitude1);
      printf("sealevel value = %d", pck.sealevel);
      printf("temperature = %f\n", pck.temperature1);
      printf("humidity value = %f\n", pck.humidity); //+- 2%
      printf("temperature value = %f\n", pck.temperature2); //SD: 0.1 degC
      printf("compensated RH value = %f\n", pck.compRH);
      printf("AIN0 is: %fV\n", pck.gas1);
      printf("AIN1 is: %fV\n", pck.gas2);
      printf("servo angle: %d\n", pck.servo_ang);
      printf("latitude is: %f\n", pck.location[0]);
      printf("longitude is: %f\n", pck.location[1]);
      struct tm ascify = (struct tm){pck.time.tm_sec, pck.time.tm_min, pck.time.tm_hour, pck.time.tm_mday, pck.time.tm_mon, pck.time.tm_year, pck.time.tm_wday, pck.time.tm_yday};
      printf("The time is: %s\n", asctime(&ascify));
      printf("ground speed: %f\n", pck.speed);
      printf("course: %f\n", pck.course);
      printf("fix quality: %d\n", pck.fix_quality);
      printf("satelites: %d\n", pck.satelites);
      printf("horizontal dilution of precision: %f\n", pck.hdop);
      printf("altitude: %f\n", pck.altitude2);
      printf("ellipsoid seperation: %f\n", pck.ellipsoid_seperation);

      printf("\n\n");

//      char *p = comms_PackMessage(pck);
//      mraa_gpio_isr_exit(irq_gpio);
//      rfm69_send(p, sizeof(struct comms_Packet));
//      free(p);

      int comms_pckcodes = (sizeof(struct comms_Packet)*CHAR_BIT)/comms_codelen;
      int encodedwordlen = pow(2, comms_codelen-1);

      char *p = comms_EncodeMessage(conv.values);
      rfm69_send(p, (encodedwordlen*comms_pckcodes)/CHAR_BIT);
      free(p);

//      rfm69_rxmode(sizeof(struct comms_Control));
//      mraa_gpio_isr(irq_gpio, MRAA_GPIO_EDGE_FALLING, control_received, NULL);
      
      usleep(1000000);

    }
    return EXIT_SUCCESS;
}

/*void control_received() {
  ctrl = comms_UnpackControl(rfm69_receive(sizeof(struct comms_Control)));
  comms_codelen = ctrl.codelen;
  servo_changeang(ctrl.servo_ang);
}*/
