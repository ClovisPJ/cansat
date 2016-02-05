#include <stdio.h>
#include <time.h> 
#include <stddef.h>
#include <gsl/gsl_matrix.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include "comms.h"

typedef uint8_t encoded_word;

int comms_sendMessage(char *data, int len) {

  int uart;
  uart = open(comms_address, O_WRONLY);
  char *header = "$ZYSK";
  int header_len = 5;

  char *buffer = malloc(len+header_len);
  strncpy(buffer, header, header_len);
  strncpy(buffer+header_len, data, len);
  write(uart, buffer, len+header_len);

  close(uart);
  return EXIT_SUCCESS;
}

char *comms_receiveMessage(int len) {

  int uart;
  uart = open(comms_address, O_RDONLY);
  char *header = "ZYSK";
  int header_len = 4;
  char *data = malloc(len);

  char *buffer = malloc(1);
  while (1) {
    read(uart, buffer, 1);
    if (*buffer == (int)'$') {
      buffer = malloc(len+header_len);
      read(uart, buffer, len+header_len);
      if (strncmp(buffer, header, header_len) == 0) {
        strncpy(data, buffer+header_len, len); 
        break;
      }
      //usleep(500000);
    }
  }

  close(uart);
  return data;
}

char *comms_PackMessage(struct comms_Packet pck) {

  union {
    struct comms_Packet packet;
    char values[sizeof(struct comms_Packet)];
  } conv;
  conv.packet = pck;

  return strndup(conv.values, sizeof(struct comms_Packet));

}

struct comms_Packet comms_UnpackMessage(char *values) {

  union {
    struct comms_Packet packet;
    char values[sizeof(struct comms_Packet)];
  } conv;
  strncpy(conv.values, values, sizeof(struct comms_Packet));

  return conv.packet;

}

char *comms_EncodeMessage(struct comms_Packet pck) {

  int pckcodes = (sizeof(struct comms_Packet)*CHAR_BIT)/comms_codelen;

  union {
    struct comms_Packet packet;
    char values[sizeof(struct comms_Packet)];
  } conv;
  conv.packet = pck;

  encoded_word *had;
  had = comms_hadamard(comms_codelen); //comms_codelen is max length of input

  struct {
    char bit : 1;
  } bits[sizeof(struct comms_Packet)*CHAR_BIT];

  for (int i = 0; i < sizeof(struct comms_Packet); i++) {
    for (int j = CHAR_BIT-1; j >= 0; j--) {
      bits[i*CHAR_BIT+(CHAR_BIT-(j+1))].bit = (conv.values[i] >> j)&1;
    }
  } 

  encoded_word unencoded[pckcodes];
  for (int i = 0; i < pckcodes; i++) {
    unencoded[i] = 0;
    for (int j = 0; j < comms_codelen; j++) {
      unencoded[i] <<= 1;
      unencoded[i] += (bits[i*comms_codelen+j].bit)&1;
    }
  }

  union {
    encoded_word codes[pckcodes];// : int(pow(2,comms_codelen-1));
    char values[(int)(pow(2,comms_codelen-1)*pckcodes)];
  } encoded;

  for (int i = 0; i < pckcodes; i++) {
    encoded.codes[i] = 0;
    encoded.codes[i] = had[unencoded[i]];
  }

  return strndup(encoded.values, pow(2,comms_codelen-1)*pckcodes);
}

struct comms_Packet comms_DecodeMessage(char *buffer) {

  int pckcodes = (sizeof(struct comms_Packet)*CHAR_BIT)/comms_codelen;

  union {
    encoded_word codes[pckcodes];// : int(pow(2,comms_codelen-1));
    char values[(int)(pow(2,comms_codelen-1)*pckcodes)];
  } encoded;
  strncpy(encoded.values, buffer, pow(2,comms_codelen-1)*pckcodes);

  encoded_word *had;
  had = comms_hadamard(comms_codelen); //comms_codelen is max length of input

  encoded_word unencoded[pckcodes];
  
  for (int i = 0; i < pckcodes; i++) {
    int min = pow(2,comms_codelen-1);
    int imin = 0;
    for (int k = 0; k < pow(2,comms_codelen); k++) {
      int hd = comms_hammingdistance(encoded.codes[i],had[k]);
      //printf("\nhd = %d",hd);
      if (hd < min) {
        min = hd;
        imin = k;
      }
    }
    unencoded[i] = 0;
    unencoded[i] = imin;
    //printf("hamdist: %d\n",min);
  }

  struct {
    char bit : 1;
  } bits[sizeof(struct comms_Packet)*CHAR_BIT];

  for (int i = 0; i < pckcodes; i++) {
      for (int j = comms_codelen-1; j >= 0 ; j--) {
        bits[comms_codelen*i+(comms_codelen-(j+1))].bit = (unencoded[i]>>j)&1;
      }
  }

  union {
    struct comms_Packet packet;
    char values[sizeof(struct comms_Packet)];
  } conv;

  for (int i = 0; i < sizeof(conv.values); i++) {
    for (int j = CHAR_BIT-1; j >= 0; j--) {
      conv.values[i] <<= 1;
      conv.values[i] += bits[i*CHAR_BIT+j].bit;
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

encoded_word *comms_hadamard(int comms_codelen) {
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

  encoded_word *comms_hadamard  = (encoded_word*) malloc(sizeof(encoded_word)*pow(2,comms_codelen));
  double value;

  for (int i = 0; i < pow(2,comms_codelen); i++) {
    *(comms_hadamard+i) = 0;
    for (int j = 0; j < pow(2,comms_codelen-1); j++) {
      *(comms_hadamard+i) <<= 1;
      if (i < pow(2,comms_codelen-1)) {
        value = gsl_matrix_get(matrixTop,i,j);
      } else {
        value = gsl_matrix_get(matrixBottom,i-pow(2,comms_codelen-1),j);
      }
      if (value == 1) {
        *(comms_hadamard+i) += 1;
        //printf("1");
      } else {
        //printf("0");
      }
    }
    //printf(" %lu ",*(comms_hadamard+i));
    //printf("\n");
  }
  //printf("\n");
  //printf("\n");

  encoded_word *p;
  p = comms_hadamard;

  return p;
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

