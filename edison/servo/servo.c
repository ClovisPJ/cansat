#include <mraa.h>

int main() {
  mraa_pwm_context pwm;
  pwm = mraa_pwm_init(0);
  if (pwm == NULL) {
      return 1;
  }

  mraa_pwm_period_ms(pwm, 20);
  mraa_pwm_pulsewidth_us(pwm, 1000);
  mraa_pwm_enable(pwm, 1);

  for (int i = 2000; i >= 1000; i--) {
    mraa_pwm_pulsewidth_us(pwm, i);
    usleep(10000);
  }
  mraa_pwm_enable(pwm, 0);

  mraa_pwm_close(pwm);
}
