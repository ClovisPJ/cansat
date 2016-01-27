#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

gsl_matrix *pseudo_inverse(gsl_matrix* input) {
  int m = input->size1;
  int n = input->size2;
  gsl_matrix *U = gsl_matrix_calloc(m,n);
  gsl_matrix_memcpy(U, input);
  gsl_matrix *V = gsl_matrix_calloc(n,n);
  gsl_vector *sigma = gsl_vector_calloc(n);
  gsl_vector *tmp = gsl_vector_calloc(n);
  gsl_linalg_SV_decomp(U, V, sigma, tmp);
  for (int i = 0; i < n; i++) {
    double s = gsl_vector_get(sigma, i);
    if (s > 0.0000000001) {
      gsl_vector_set(sigma, i, 1/s);
    } else if (s > 0) {
      gsl_vector_set(sigma, i, 0);
    }
  }
  gsl_matrix *tmpa = gsl_matrix_calloc(n,n);
  gsl_matrix *tmpb = gsl_matrix_calloc(n,m);
  gsl_matrix *tmpc = gsl_matrix_calloc(n,m);
  for (int i = 0; i < n; i++) {
    gsl_matrix_set(tmpa, i, i, gsl_vector_get(sigma, i));
  }
  gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1, tmpa, U, 0, tmpb);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, V, tmpb, 0, tmpc);
  return tmpc;
}

int main() {

  int states = 6;
  int unfiltered_states = 3;

  gsl_matrix *state_mean = gsl_matrix_calloc(states,1);
  gsl_matrix_set(state_mean, 0,0, 1);
  gsl_matrix *state_covariance = gsl_matrix_calloc(states,states);

  gsl_matrix *observation_mean = gsl_matrix_calloc(states,1);
  gsl_matrix *observation_covariance = gsl_matrix_calloc(states,states);
  gsl_matrix_set_all(observation_covariance, 1);

  gsl_matrix *observation_transformation = gsl_matrix_calloc(states,states);
  
  gsl_matrix *estimate_mean = gsl_matrix_calloc(states,1);
  gsl_matrix *estimate_covariance = gsl_matrix_calloc(states,states);
  gsl_matrix *kalman_gain = gsl_matrix_calloc(states,states);

  gsl_matrix *temp21a = gsl_matrix_calloc(states,1);
  gsl_matrix *temp21b = gsl_matrix_calloc(states,1);
  gsl_matrix *temp22a = gsl_matrix_calloc(states,states);
  gsl_matrix *temp22b = gsl_matrix_calloc(states,states);

  gsl_matrix *predict = gsl_matrix_calloc(states,states);
  gsl_matrix_set(predict, 0, 0, 1);
  gsl_matrix_set(predict, 1, 1, 1);
  gsl_matrix_set(predict, 2, 2, 1);
  gsl_matrix_set(predict, 3, 3, 1);
  gsl_matrix_set(predict, 4, 4, 1);
  gsl_matrix_set(predict, 5, 5, 1);
  //gsl_matrix_set(predict, 0, 3, timestep);
  //gsl_matrix_set(predict, 1, 4, timestep);
  //gsl_matrix_set(predict, 2, 5, timestep);

  gsl_matrix *control = gsl_matrix_calloc(states, unfiltered_states);
  //gsl_matrix_set(control, 0, 0, 0.5*pow(timestep,2));
  //gsl_matrix_set(control, 1, 1, 0.5*pow(timestep,2));
  //gsl_matrix_set(control, 2, 2, 0.5*pow(timestep,2));
  //gsl_matrix_set(control, 3, 0, timestep);
  //gsl_matrix_set(control, 4, 1, timestep);
  //gsl_matrix_set(control, 5, 2, timestep);

  gsl_vector *acceleration = gsl_vector_calloc(unfiltered_states);
  gsl_vector *velocity = gsl_vector_calloc(unfiltered_states);
  gsl_vector *location = gsl_vector_calloc(unfiltered_states);

  double timestep;

  int s;

  for (int t = 1; t < 100; t++) {

    timestep = 1;
    gsl_vector_set(acceleration, 0, 1);
    gsl_vector_set(acceleration, 1, 0);
    gsl_vector_set(acceleration, 2, 0);
    gsl_vector_add(velocity, acceleration);
    gsl_vector_add(location, velocity);

    gsl_matrix_set(predict, 0, 3, timestep);
    gsl_matrix_set(predict, 1, 4, timestep);
    gsl_matrix_set(predict, 2, 5, timestep);
    gsl_matrix_set(control, 0, 0, 0.5*pow(timestep,2));
    gsl_matrix_set(control, 1, 1, 0.5*pow(timestep,2));
    gsl_matrix_set(control, 2, 2, 0.5*pow(timestep,2));
    gsl_matrix_set(control, 3, 0, timestep);
    gsl_matrix_set(control, 4, 1, timestep);
    gsl_matrix_set(control, 5, 2, timestep);

    //observation_transformation = observation_mean[k] * pseudoinverse(state_mean[k-1])
    gsl_matrix_set_zero(temp21a);
    gsl_matrix_set(temp21a, 0, 0, gsl_vector_get(location,0));
    gsl_matrix_set(temp21a, 1, 0, gsl_vector_get(location,1));
    gsl_matrix_set(temp21a, 2, 0, gsl_vector_get(location,2));
    gsl_matrix_set(temp21a, 3, 0, gsl_vector_get(velocity,0));
    gsl_matrix_set(temp21a, 4, 0, gsl_vector_get(velocity,1));
    gsl_matrix_set(temp21a, 5, 0, gsl_vector_get(velocity,2));
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, temp21a, pseudo_inverse(state_mean), 0, observation_transformation);

    //observation_mean[k] = observation_transformation * state_mean[k-1]
    gsl_matrix_memcpy(temp21a, state_mean);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, observation_transformation, temp21a, 0, observation_mean);

    //observation_covariance[k] = observation_transformation * state_covariance * transpose(observation_transformation)
/*    gsl_matrix_set_zero(temp22a);
    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1, state_covariance, observation_transformation, 0, temp22a); //notice observation_transformation is transposed
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, observation_transformation, temp22a, 0, observation_covariance);
*/
    //estimate_mean = predict * state_mean + control * acceleration;
    gsl_matrix_set_zero(estimate_mean);
    gsl_matrix_set_zero(temp21a);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, predict, state_mean, 0, estimate_mean);
    gsl_matrix_view test = gsl_matrix_view_vector(acceleration, unfiltered_states, 1);
    gsl_matrix * tmp = &test.matrix;
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, control, tmp, 0, temp21a);
    gsl_matrix_add(estimate_mean, temp21a);

    //estimate_covariance = predict * state_covariance * transpose(predict) + noise;
    gsl_matrix_set_zero(estimate_covariance);
    gsl_matrix_set_zero(temp22a);
    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1, state_covariance, predict, 0, temp22a); //notice predict is transposed
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, predict, temp22a, 0, estimate_covariance);
    gsl_matrix_add_constant(estimate_covariance, 0.1); //adxl345 noise, this is completely wrong
    
    //kalman_gain = estimate_covariance * pseudoinverse(estimate_covariance + observation_covariance);
    gsl_matrix_set_zero(kalman_gain);
    gsl_matrix_set_zero(temp22a);
    gsl_matrix_set_zero(temp22b);
    gsl_matrix_memcpy(temp22a, observation_covariance);
    gsl_matrix_add(temp22a, estimate_covariance);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, estimate_covariance, pseudo_inverse(temp22b), 0, kalman_gain);
    
    //state_mean = estimate_mean +  kalman_gain * ( observation_mean - estimate_mean );
    gsl_matrix_set_zero(state_mean);
    gsl_matrix_set_zero(temp21a);
    gsl_matrix_memcpy(temp21a, observation_mean);
    gsl_matrix_sub(temp21a, estimate_mean);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, kalman_gain, temp21a, 0, state_mean);
    gsl_matrix_add(state_mean, estimate_mean);
    
    //state_covariance = estimate_covariance - kalman_gain * ( estimate_covariance );
    gsl_matrix_set_zero(state_covariance);
    gsl_matrix_set_zero(temp22a);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, kalman_gain, estimate_covariance, 0, temp22a);
    gsl_matrix_add(state_covariance, estimate_covariance);
    gsl_matrix_sub(state_covariance, temp22a);

    printf("state_mean:");
    gsl_matrix_fprintf(stdout, state_mean, "%f");
/*    printf("state_covariance:");
    gsl_matrix_fprintf(stdout, state_covariance, "%f");
*/
    printf("observation_mean:");
    gsl_matrix_fprintf(stdout, observation_mean, "%f");
/*    printf("observation_covariance:");
    gsl_matrix_fprintf(stdout, observation_covariance, "%f");
*/
    printf("estimate_mean:");
    gsl_matrix_fprintf(stdout, estimate_mean, "%f");
/*    printf("estimate_covariance:");
    gsl_matrix_fprintf(stdout, estimate_covariance, "%f");
*/
    printf("\n");

    usleep(1000000);
  
  }
}
