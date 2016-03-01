#include <mraa.h>

mraa_pwm_context pwm;

int servo_ang; // -180 to 180

int servo_init();
void servo_changeang(int ang);
void servo_down();
