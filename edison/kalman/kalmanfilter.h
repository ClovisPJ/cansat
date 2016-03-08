#include <gsl/gsl_matrix.h>

int kalmanfilter_setup();
int kalmanfilter_step(gsl_matrix* loc, gsl_matrix* velo, gsl_matrix* accel, float tstep, float hdp, float v_err);
gsl_matrix *kalmanfilter_matrix(float *values);
gsl_matrix *kalmanfilter_pseudo_inverse(gsl_matrix* input);

int states;
int unfiltered_states;

gsl_matrix *state_mean;
gsl_matrix *state_covariance;
gsl_matrix *observation_mean;
gsl_matrix *observation_covariance;
gsl_matrix *observation_transformation;
gsl_matrix *estimate_mean;
gsl_matrix *estimate_covariance;
gsl_matrix *kalman_gain;
gsl_matrix *temp21a;
gsl_matrix *temp21b;
gsl_matrix *temp22a;
gsl_matrix *temp22b;
gsl_matrix *predict;
gsl_matrix *control;
gsl_matrix *acceleration;
gsl_matrix *velocity;
gsl_matrix *location;
gsl_matrix *delta_location;
gsl_matrix *temp_location;
gsl_matrix *pinv;

double timestep;
float hdop;
float vert_err;

int s;
