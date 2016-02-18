#include <stdio.h>
#include <time.h> 
#include <stddef.h>
#include <gsl/gsl_matrix.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "comms.h"
#include "uart.h"

typedef uint8_t encoded_word;

char *comms_PackMessage(struct comms_Packet pck) {

  union {
    struct comms_Packet packet;
    char values[sizeof(struct comms_Packet)];
  } conv;
  conv.packet = pck;

  char *ret = malloc(sizeof(struct comms_Packet));
  memcpy(ret, conv.values, sizeof(struct comms_Packet));
  return ret;
}

struct comms_Packet comms_UnpackMessage(char *values) {

  union {
    struct comms_Packet packet;
    char values[sizeof(struct comms_Packet)];
  } conv;
  memcpy(conv.values, values, sizeof(struct comms_Packet));

  return conv.packet;

}

char *comms_EncodeMessage(struct comms_Packet pck) {

  int pckcodes = (sizeof(struct comms_Packet)*CHAR_BIT)/comms_codelen;

  union {
    struct comms_Packet packet;
    char values[sizeof(struct comms_Packet)];
  } conv;
  conv.packet = pck;

  int bits[sizeof(struct comms_Packet)*CHAR_BIT];

  for (int i = 0; i < sizeof(struct comms_Packet); i++) {
    for (int j = CHAR_BIT-1; j >= 0; j--) {
      bits[i*CHAR_BIT+(CHAR_BIT-(j+1))] = (conv.values[i] >> j)&1;
    }
  } 


  int unencoded[pckcodes][comms_codelen];
  for (int i = 0; i < pckcodes; i++) {
    for (int j = 0; j < comms_codelen; j++) {
      unencoded[i][j] = (bits[j+i*comms_codelen])&1;
    }
  }

  union {
    encoded_word codes[pckcodes];// : int(pow(2,comms_codelen-1));
    char values[(int)(pow(2,comms_codelen-1)*pckcodes/CHAR_BIT)];
  } encoded;

  int **hadamard;
  hadamard = comms_hadamard(comms_codelen); //comms_codelen is max length of input

  for (int i = 0; i < pckcodes; i++) {
    int index = 0; // max is 2^comms_codelen
    for (int j = 0; j < comms_codelen; j++) { // get index of code
      index += (unencoded[i][comms_codelen-1-j]<<j);
    }
    encoded.codes[i] = 0;
    for (int j = 0; j < pow(2, comms_codelen-1); j++) { // get code from index
      encoded.codes[i] += (hadamard[index][(int)pow(2,comms_codelen-1)-j-1]<<j);
    }
  }

  char *ret = malloc(pow(2,comms_codelen-1)*pckcodes/CHAR_BIT);
  memcpy(ret, encoded.values, pow(2,comms_codelen-1)*pckcodes/CHAR_BIT);
  return ret;
}

struct comms_Packet comms_DecodeMessage(char *buffer) {

  int pckcodes = (sizeof(struct comms_Packet)*CHAR_BIT)/comms_codelen;

  union {
    encoded_word codes[pckcodes];// : int(pow(2,comms_codelen-1));
    char values[(int)(pow(2,comms_codelen-1)*pckcodes)];
  } encoded;
  memcpy(encoded.values, buffer, pow(2,comms_codelen-1)*pckcodes);

  int **hadamard;
  hadamard = comms_hadamard(comms_codelen); //comms_codelen is max length of input

  int unencoded[pckcodes]; // max is comms_codelen

  for (int i = 0; i < pckcodes; i++) {
    int min = pow(2,comms_codelen-1); // max error is this, but any big number is fine
    int imin = 0;
    for (int j = 0; j < pow(2,comms_codelen); j++) {
      encoded_word had = 0;
      for (int k = 0; k < pow(2,comms_codelen-1); k++) { // get j'th code
        had += (hadamard[j][(int)pow(2,comms_codelen-1)-k-1]<<k);
      }
      int hd = comms_hammingdistance(encoded.codes[i], had); // then match to find closest
      //printf("\nhd = %d",hd);
      if (hd < min) {
        min = hd;
        imin = j;
      }
    }
    unencoded[i] = 0;
    unencoded[i] = imin;
    //printf("hamdist: %d\n",min);
  }

  int bits[sizeof(struct comms_Packet)*CHAR_BIT];

  for (int i = 0; i < pckcodes; i++) {
      for (int j = comms_codelen-1; j >= 0 ; j--) {
        bits[comms_codelen*i+(comms_codelen-(j+1))] = (unencoded[i]>>j)&1;
      }
  }

  union {
    struct comms_Packet packet;
    char values[sizeof(struct comms_Packet)];
  } conv;

  for (int i = 0; i < sizeof(struct comms_Packet); i++) {
    conv.values[i] = 0;
    for (int j = 0; j < CHAR_BIT; j++) {
      conv.values[i] += bits[i*CHAR_BIT+j] << (CHAR_BIT-1-j);
    }
  }

  return conv.packet;
}


int comms_hammingdistance(encoded_word a, encoded_word b) {
  encoded_word z = a^b;
  int n = 0;
  while (z) {
    if (z&1){
      n++;
    }
    z >>= 1;
  }
  return n;
}

int **comms_hadamard(int comms_codelen) {
  gsl_matrix *initTop = gsl_matrix_alloc(2,2);
  initTop->tda = 2;
  gsl_matrix_set(initTop,0,0,1); //start of top half with matrix:
  gsl_matrix_set(initTop,0,1,1); // 1  1
  gsl_matrix_set(initTop,1,0,1); // 1 -1
  gsl_matrix_set(initTop,1,1,-1);

  gsl_matrix *matrixTop = gsl_matrix_alloc(1,1);
  matrixTop->tda = 1;
  gsl_matrix_set(matrixTop,0,0,1);

  for (int i = 0; i < comms_codelen-1; i++) { //comms_codelen is -1 as t&b allow *2 the codes
    matrixTop = comms_KPro(matrixTop,initTop);
  }

  gsl_matrix *initBottom = gsl_matrix_alloc(2,2);
  initBottom->tda = 2;
  gsl_matrix_set(initBottom,0,0,-1); //start of bottom half with matrix:
  gsl_matrix_set(initBottom,0,1,-1); // -1 -1
  gsl_matrix_set(initBottom,1,0,-1); // -1  1
  gsl_matrix_set(initBottom,1,1,1);

  gsl_matrix *matrixBottom = gsl_matrix_alloc(1,1);
  matrixBottom->tda = 1;
  gsl_matrix_set(matrixBottom,0,0,1);

  for (int i = 0; i < comms_codelen-1; i++) {
    matrixBottom = comms_KPro(matrixBottom,initBottom);
  }

  int **hadamard = calloc(pow(2, comms_codelen), sizeof(int*));
  for (int i = 0; i < pow(2, comms_codelen); i++) {
    hadamard[i] = calloc(pow(2,comms_codelen-1), sizeof(int));
  }

  double value;

  for (int i = 0; i < pow(2, comms_codelen); i++) {
    for (int j = 0; j < pow(2, comms_codelen-1); j++) {
      if (i < pow(2,comms_codelen-1)) {
        value = gsl_matrix_get(matrixTop,i,j); //take from the top half
      } else {
        value = gsl_matrix_get(matrixBottom,i-pow(2,comms_codelen-1),j); //then the bottom half
      }
      if (value == 1) {
        hadamard[i][j] = 1;
       // printf("1");
      } else {
        hadamard[i][j] = 0;
       // printf("0");
      }
    }
    //printf("\n");
  } 
     // printf("\n");
     // printf("\n");

  return hadamard;
}

/*sourced from https://stackoverflow.com/questions/13722543/efficient-way-to-compute-kronecker-product-of-matrices-with-gsl*/

gsl_matrix *comms_KPro(gsl_matrix *a, gsl_matrix *b) {
    int i, j, k, l;
    int m, p, n, q;
    m = a->size1;
    p = a->size2;
    n = b->size1;
    q = b->size2;

    gsl_matrix *c = gsl_matrix_alloc(m*n, p*q);
    double da, db;

     for (i = 0; i < m; i++)    {
          for (j = 0; j < p; j++)   {
              da = gsl_matrix_get (a, i, j);
              for (k = 0; k < n; k++)   {
                  for (l = 0; l < q; l++)   {
                      db = gsl_matrix_get (b, k, l);
                      gsl_matrix_set (c, n*i+k, q*j+l, da * db);                
                  }
              }
          }
      }

    return c;
}

int comms_printbin (encoded_word dec) {
  for (int i = (CHAR_BIT*sizeof(encoded_word))-1; i >= 0; i--) {
    if (((dec>>i)&1)==1) {
      printf("1");
    } else {
      printf("0");
    }
  }
  return EXIT_SUCCESS;
}

