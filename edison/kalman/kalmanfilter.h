#include <gsl/gsl_matrix.h>

int kalmanfilter_setup();
int kalmanfilter_step(gsl_matrix* loc, gsl_matrix* velo, gsl_matrix* accel, float tstep, float hdp, float v_err);
gsl_matrix *kalmanfilter_matrix(float *values);
gsl_matrix *kalmanfilter_pseudo_inverse(gsl_matrix* input);
