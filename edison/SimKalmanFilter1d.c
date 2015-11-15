#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <gsl.h>

int main() {

  float state_mean = 0; //eg position
  float state_variance = 0;

  float actual_mean = 0;
  
  float noise = 100;
  time_t t;
  srand((unsigned) time(&t));
  
  for (int t = 0; t < 100; t++) {

    float rannum;
    rannum = (rand() % 2000) - 1000;
    rannum *= 0.001;
  
    float observation_mean = actual_mean + noise*rannum;
    float observation_variance = noise;
    //mraa
    
    float estimate_mean = 0;
    float estimate_variance = 0;
    //estimate_mean = state_mean * Fk + acceleration * Bk;
    //estimate_variance = Fk * state_variance * transpose(Fk) + noise;
    estimate_mean = state_mean + 10; //10 is v
    estimate_variance = state_variance + noise;
    
    printf("    Observation: %f,  %f", observation_mean, observation_variance);
    printf("    Estimate: %f,  %f", estimate_mean, estimate_variance);

    float kalman_gain;
    kalman_gain = estimate_variance / (estimate_variance + observation_variance);
    
    state_mean = estimate_mean +  kalman_gain * ( observation_mean - estimate_mean );
    
    state_variance = estimate_variance - kalman_gain * ( estimate_variance );
  
    printf("     State: %f,   %f", state_mean, state_variance);
    printf("     Actual: %f\n", actual_mean);
  
    actual_mean += 10;
  
  }

}
