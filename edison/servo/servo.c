#include <mraa.h>

#include "servo.h"

int servo_init() {
  servo_pwm = mraa_pwm_init(0);
  if (servo_pwm == NULL) {
      return 1;
  }
  servo_changeang(0);
}

void servo_changeang(int ang) {
  if ((ang < -180) || (ang > 180)) return;
  servo_ang = ang;
  
  ang += 180; // 0 - 360
  ang *= 1000/360; // 0 - 1000
  ang += 1000; // 1000 - 2000

  mraa_pwm_period_us(servo_pwm, 20000-ang);
  mraa_pwm_pulsewidth_us(servo_pwm, ang);
  mraa_pwm_enable(servo_pwm, 1);
}

void servo_down() {
  mraa_pwm_enable(servo_pwm, 0);
}
