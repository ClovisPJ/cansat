#include <gsl/gsl_matrix.h>

int kalmanfilter_setup();
int kalmanfilter_step(gsl_matrix* location, gsl_matrix* velocity, gsl_matrix* acceleration, float timestep);
gsl_matrix *matrix(float *values);
gsl_matrix *pseudo_inverse(gsl_matrix* input);
