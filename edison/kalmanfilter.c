#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

int main() {

  gsl_matrix *state_mean = gsl_matrix_calloc(2,1);
  gsl_matrix *state_covariance = gsl_matrix_calloc(2,2);

  gsl_matrix *actual_mean = gsl_matrix_calloc(2,1);
  
  float noise = 10;
  time_t t;
  srand((unsigned) time(&t));

  gsl_matrix *observation_mean = gsl_matrix_calloc(2,1);
  gsl_matrix *observation_covariance = gsl_matrix_calloc(2,2);
  
  gsl_matrix *estimate_mean = gsl_matrix_calloc(2,1);
  gsl_matrix *estimate_covariance = gsl_matrix_calloc(2,2);
  gsl_matrix *kalman_gain = gsl_matrix_calloc(2,2);

  gsl_matrix *temp1 = gsl_matrix_calloc(2,1);
  gsl_matrix *temp2 = gsl_matrix_calloc(2,2);
  gsl_matrix *temp3 = gsl_matrix_calloc(2,2);

  for (int t = 0; t < 100; t++) {

    float rannum;
    rannum = (rand() % 2000) - 1000;
    rannum *= 0.001;
  
    gsl_matrix_memcpy(observation_mean, actual_mean);
    gsl_matrix_add_constant(observation_mean, noise*rannum);

    gsl_matrix_set_all(observation_covariance, noise);
    //mraa
    
    //estimate_mean = state_mean * Fk + acceleration * Bk;
    //estimate_variance = Fk * state_variance * transpose(Fk) + noise;
    gsl_matrix_memcpy(estimate_mean, state_mean);
    gsl_matrix_add_constant(estimate_mean, 10); //10 is v

    gsl_matrix_memcpy(estimate_covariance, state_covariance);
    gsl_matrix_add_constant(estimate_covariance, noise*0.01);
    
    int val; //quick fix due to singular matrices
    val = gsl_matrix_get(observation_covariance, 0,0);
    gsl_matrix_set(observation_covariance, 0,0, val+0.001*rannum);
    
    //kalman_gain = estimate_covariance * inv(estimate_covariance + observation_covariance);
    gsl_matrix_set_zero(kalman_gain);
    //gsl_matrix_set_zero(temp1);
    gsl_matrix_set_zero(temp2);
    gsl_matrix_set_zero(temp3);
    gsl_matrix_add(temp2, observation_covariance);
    gsl_matrix_add(temp2, estimate_covariance);
    gsl_permutation * p = gsl_permutation_alloc (2);
    int s;
    gsl_linalg_LU_decomp(temp2, p, &s);
    /*if (gsl_linalg_LU_det(temp2, s) == 0) { //better fix for singular matrices but not perfect
      printf("Matrix Singular");
      int val; 
      val = gsl_matrix_get(temp2, 0,0);
      gsl_matrix_set(temp2, 0,0, val+rannum);
    }*/
    gsl_linalg_LU_invert(temp2, p, temp3);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, temp3, estimate_covariance, 0, kalman_gain);
    
    //state_mean = estimate_mean +  kalman_gain * ( observation_mean - estimate_mean );
    gsl_matrix_set_zero(state_mean);
    gsl_matrix_set_zero(temp1);
    //gsl_matrix_set_zero(temp2);
    gsl_matrix_add(temp1, observation_mean);
    gsl_matrix_sub(temp1, estimate_mean);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, kalman_gain, temp1, 0, state_mean);
    gsl_matrix_add(state_mean, estimate_mean);
    
    //state_covariance = estimate_covariance - kalman_gain * ( estimate_covariance );
    gsl_matrix_set_zero(state_covariance);
    //gsl_matrix_set_zero(temp1);
    gsl_matrix_set_zero(temp2);
    gsl_matrix_memcpy(temp2, estimate_covariance);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, temp2, kalman_gain, 0, temp2);
    gsl_matrix_add(state_covariance, estimate_covariance);
    gsl_matrix_sub(state_covariance, temp2);
  
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

    gsl_matrix_add_constant(actual_mean,10);
    usleep(1000000);
  
  }

}
