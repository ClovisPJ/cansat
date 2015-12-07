#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

int main() {

  gsl_matrix *state_mean = gsl_matrix_calloc(2,1);
  gsl_matrix *state_covariance = gsl_matrix_calloc(2,2);

  gsl_matrix *actual_mean = gsl_matrix_calloc(2,1);
  
  double noise = 10;
  time_t t;
  srand((unsigned) time(&t));

  double timestep = 1;
  double acceleration;

  gsl_matrix *observation_mean = gsl_matrix_calloc(2,1);
  gsl_matrix *observation_covariance = gsl_matrix_calloc(2,2);
  
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

  for (int t = 0; t < 100; t++) {

    double rannum;
    rannum = (rand() % 1000);
    rannum *= 0.001;
  
    gsl_matrix_memcpy(observation_mean, actual_mean); //fake poisition reading (GPS), fake velocity reading (diff(GPS))
    gsl_matrix_add_constant(observation_mean, noise*rannum); 

    gsl_matrix_set_all(observation_covariance, noise); //adxl345 noise
    acceleration = 0; //adxl345
    
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
    gsl_matrix_add_constant(estimate_covariance, noise+0.001*rannum);
    
    int val; //quick fix due to singular matrices
    val = gsl_matrix_get(observation_covariance, 0,0);
    gsl_matrix_set(observation_covariance, 0,0, val+0.001*rannum);
    
    //kalman_gain = estimate_covariance * inv(estimate_covariance + observation_covariance);
    gsl_matrix_set_zero(kalman_gain);
    gsl_matrix_set_zero(temp22a);
    gsl_matrix_set_zero(temp22b);
    gsl_matrix_memcpy(temp22a, observation_covariance);
    gsl_matrix_add(temp22a, estimate_covariance);
    gsl_permutation * p = gsl_permutation_alloc (2);
    int s;
    gsl_linalg_LU_decomp(temp22a, p, &s);
    /*if (gsl_linalg_LU_det(temp22a, s) == 0) { //better fix for singular matrices but not perfect
      printf("Matrix Singular");
      int val; 
      val = gsl_matrix_get(temp22a, 0,0);
      gsl_matrix_set(temp22a, 0,0, val+rannum);
    }*/
    gsl_linalg_LU_invert(temp22a, p, temp22b);
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

    printf("\nactual_mean:\n");
    gsl_matrix_fprintf(stdout, actual_mean, "%f");
  
    printf("\n\n\n");

    double val2;
    val2 = gsl_matrix_get(actual_mean, 0,0);
    gsl_matrix_set(actual_mean, 0,0, val2+2);
    gsl_matrix_set(actual_mean, 1,0, 2);
    usleep(1000000);
  
  }

}
