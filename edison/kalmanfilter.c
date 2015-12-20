#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

#include "gps.h"
#include "adxl345.h"

int main() {

  gsl_matrix *state_mean = gsl_matrix_calloc(2,1);
  gsl_matrix *state_covariance = gsl_matrix_calloc(2,2);

  double timestep;
  double acceleration;

  gsl_matrix *observation_mean = gsl_matrix_calloc(2,1);
  gsl_matrix *observation_covariance = gsl_matrix_calloc(2,2);
  gsl_matrix *observation_transformation = gsl_matrix_calloc(2,2);
  
  gsl_matrix *estimate_mean = gsl_matrix_calloc(2,1);
  gsl_matrix *estimate_covariance = gsl_matrix_calloc(2,2);
  gsl_matrix *kalman_gain = gsl_matrix_calloc(2,2);

  gsl_matrix *temp21a = gsl_matrix_calloc(2,1);
  gsl_matrix *temp21b = gsl_matrix_calloc(2,1);
  gsl_matrix *temp22a = gsl_matrix_calloc(2,2);
  gsl_matrix *temp22b = gsl_matrix_calloc(2,2);

  gsl_matrix *predict = gsl_matrix_calloc(2,2);
  gsl_matrix_set(predict, 0,0, 1);
  gsl_matrix_set(predict, 1,1, 1);
  gsl_matrix_set(predict, 0,1, timestep);

  gsl_matrix *control = gsl_matrix_calloc(2,1);
  gsl_matrix_set(control, 0,0, 0.5*pow(timestep,2));
  gsl_matrix_set(control, 1,0, timestep);


  gps_init();
  gps_locate();
  adxl345_init();
  float *acc;

  gsl_vector *loc = gps_get_location();
  gsl_vector *templ;
  gsl_vector *templ2;

  time_t time = gps_get_time();
  time_t tempt;

  gsl_vector *velocity = gsl_vector_calloc(2);

  int s;

  for (int t = 0; t < 100; t++) {

    gsl_vector_memcpy(templ, loc);
    loc = gps_get_location();
    gsl_vector_memcpy(templ2, loc);
    gsl_vector_sub(templ2, templ);
    gsl_vector_memcpy(velocity, templ2);

    tempt = time;
    time = gps_get_time();
    timestep = difftime(time,tempt);
    
    //observation_transformation = state_mean[k] * inverse(state_mean[k-1])
    gsl_matrix_set_zero(temp21a);
    gsl_matrix_set(temp21a, 0,0, pow(pow(gsl_vector_get(loc,0),2)+pow(gsl_vector_get(loc,1),2),0.5));
    gsl_matrix_set(temp21a, 1,0, pow(pow(gsl_vector_get(velocity,0),2)+pow(gsl_vector_get(velocity,1),2),0.5));
    gsl_permutation * p1 = gsl_permutation_alloc (2);
    s = 0;
    gsl_linalg_LU_decomp(state_mean, p1, &s);
    gsl_linalg_LU_solve(state_mean, p1, temp21a, observation_transformation);

    //observation_mean[k] = observation_transformation * observation_mean[k-1]
    gsl_matrix_memcpy(temp21a, observation_mean);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, observation_transformation, temp21a, 0, observation_mean);

    //observation_covariance[k] = observation_transformation * observation_covariance * transpose(observation_transformation)
    gsl_matrix_set_zero(temp22a);
    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1, observation_covariance, observation_transformation, 0, temp22a); //notice predict is transposed
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, observation_transformation, temp22a, 0, observation_covariance);

    adxl345_update(); // Update the data
    acc = adxl345_getacceleration(); // Read acceleration (g)
    acceleration = pow(pow(acc[0],2)+pow(acc[1],2)+pow(acc[2],2),0.5); //adxl345 SD: 1.95m
    
    //estimate_mean = predict * state_mean + control * acceleration;
    gsl_matrix_set_zero(estimate_mean);
    gsl_matrix_set_zero(temp21a);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, predict, state_mean, 0, estimate_mean);
    gsl_matrix_memcpy(temp21a, control);
    gsl_matrix_scale(temp21a, acceleration);
    gsl_matrix_add(estimate_mean, temp21a);

    //estimate_covariance = predict * state_covariance * transpose(predict) + noise;
    gsl_matrix_set_zero(estimate_covariance);
    gsl_matrix_set_zero(temp22a);
    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1, state_covariance, predict, 0, temp22a); //notice predict is transposed
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, predict, temp22a, 0, estimate_covariance);
    gsl_matrix_add_constant(estimate_covariance, 1.95); //adxl345 noise, this is completely wrong
    
    int val; //quick fix due to singular matrices
    val = gsl_matrix_get(observation_covariance, 0,0);
    gsl_matrix_set(observation_covariance, 0,0, val+0.001);
    
    //kalman_gain = estimate_covariance * inv(estimate_covariance + observation_covariance);
    gsl_matrix_set_zero(kalman_gain);
    gsl_matrix_set_zero(temp22a);
    gsl_matrix_set_zero(temp22b);
    gsl_matrix_memcpy(temp22a, observation_covariance);
    gsl_matrix_add(temp22a, estimate_covariance);
    gsl_permutation * p2 = gsl_permutation_alloc (2);
    s = 0;
    gsl_linalg_LU_decomp(temp22a, p2, &s);
    /*if (gsl_linalg_LU_det(temp22a, s) == 0) { //better fix for singular matrices but not perfect
      printf("Matrix Singular");
      int val; 
      val = gsl_matrix_get(temp22a, 0,0);
      gsl_matrix_set(temp22a, 0,0, val+rannum);
    }*/
    gsl_linalg_LU_invert(temp22a, p2, temp22b);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, estimate_covariance, temp22b, 0, kalman_gain);
    
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
  
    printf("\nstate_mean:\n");
    gsl_matrix_fprintf(stdout, state_mean, "%f");
    printf("\nstate_covariance:\n");
    gsl_matrix_fprintf(stdout, state_covariance, "%f");

    printf("\nobservation_mean:\n");
    gsl_matrix_fprintf(stdout, observation_mean, "%f");
    printf("\nobservation_covariance:\n");
    gsl_matrix_fprintf(stdout, observation_covariance, "%f");

    printf("\nestimate_mean:\n");
    gsl_matrix_fprintf(stdout, estimate_mean, "%f");
    printf("\nestimate_covariance:\n");
    gsl_matrix_fprintf(stdout, estimate_covariance, "%f");

    printf("\n\n\n");

    usleep(1000000);
  
  }

}

