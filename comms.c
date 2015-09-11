#include <stdio.h>
#include <time.h> 
#include <stddef.h>
#include <gsl/gsl_matrix.h>
#include <math.h>
#include <limits.h>

#if 0
struct packtm {
   int tm_sec : 7;        /* seconds,  range 0 to 59          */
   int tm_min : 7;         /* minutes, range 0 to 59           */
   int tm_hour : 5;        /* hours, range 0 to 23             */
   int tm_mday : 5;        /* day of the month, range 1 to 31  */
   int tm_mon : 5;         /* month, range 0 to 11             */
   int tm_year : 8;        /* The number of years since 1900   */
   int tm_wday : 3;        /* day of the week, range 0 to 6    */
   int tm_yday : 9;        /* day in the year, range 0 to 365  */
   int tm_isdst : 1;       /* daylight saving time             */
   /*50 bits, using 2 ints*/
};
#endif

#pragma pack(push) //pragma is needed to remove padding (extra spacing)
#pragma pack(1)
struct Packet {
  struct tm time;
  int type;
  int data[3];
};
#pragma pack(pop)

union Changeform {
  struct Packet packet;
  unsigned char values[sizeof(struct Packet)/sizeof(char)];
};

int sendMessage(struct Packet,int);
struct Packet receiveMessage(int);
int hammingdistance(unsigned long, unsigned long);
unsigned long *hadamard(int);
gsl_matrix *KPro(gsl_matrix *, gsl_matrix *);
int printbin(unsigned long);

int main() {

  clock_t rawtime = time(NULL);
  int type = 1;
  int input[] = {1,-2,-1};

  struct Packet pck;
  pck.time = *localtime(&rawtime);
  pck.type = type;

  int len = sizeof(input)/sizeof(input[0]);
  for (int i = 0; i < len; i++) {
    pck.data[i] = input[i];
  }

  // 6 <= codelen <= 2 
  //TODO: allow 7 (should work as fits lu, and allow 1. broken due to codes generated incorrectly
  //sendMessage(pck,2);
  receiveMessage(2);

  //sendMessage(receiveMessage(256),256);

  return 0;
}

int sendMessage(struct Packet pck, int codelen) {

  union Changeform toSend;
  toSend.packet = pck;
  for (int i = 0; i < sizeof(struct Packet); i++) {
    toSend.values[i] = 255;
  }

  unsigned long *had;
  had = hadamard(codelen); //codelen here is number of repetions, so max length of input
  int pckcodes = (sizeof(struct Packet)*CHAR_BIT)/codelen;

  struct {
    unsigned char bit : 1;
  } bits[sizeof(struct Packet)*CHAR_BIT];

  for (int i = 0; i < sizeof(struct Packet); i++) {
    for (int j = CHAR_BIT-1; j >= 0; j--) {
      bits[i*CHAR_BIT+(CHAR_BIT-(j+1))].bit = (toSend.values[i] >> j)&1;
    }
  }

  for (int i = 0; i < sizeof(bits)/sizeof(bits[0]); i++) {
    printf("%d",bits[i].bit);
  }
  printf("\n");
  printf("\n");
  unsigned long unencoded[pckcodes];
  
  for (int i = 0; i < pckcodes; i++) {
    unencoded[i] = 0;
    for (int j = 0; j < codelen; j++) {
      unencoded[i] <<= 1;
      unencoded[i] += bits[i*codelen+j].bit;
    }
  }

  for (int i = 0; i < sizeof(unencoded)/sizeof(unencoded[0]); i++) {
    printbin(unencoded[i]);
    printf("\n");
  }
  printf("\n");
  printf("\n");
  unsigned long encoded[pckcodes];

  for (int i = 0; i < pckcodes; i++) {
    encoded[i] = 0;
    encoded[i] = had[unencoded[i]];
  }
  for (int i = 0; i < sizeof(encoded)/sizeof(encoded[0]); i++) {
    printbin(encoded[i]);
    printf("\n");
  }
  printf("\n");
  printf("\n");

/*  struct tm *timeinfo;
  timeinfo = &toSend.packet.time;
  printf("%s",asctime(timeinfo)); */

  return 0;
}

struct Packet receiveMessage(int codelen) {
  union Changeform received;
  int pckcodes = (sizeof(struct Packet)*CHAR_BIT)/codelen;

  unsigned long encoded[pckcodes];
  unsigned long *had;
  had = hadamard(codelen); //codelen is length of words, generating hd with enough for that

  for (int i = 0; i < pckcodes; i++) {
    encoded[i] = 0;
    encoded[i] = had[(int)pow(2,codelen)-1]; //from receiver
  }
  for (int i = 0; i < sizeof(encoded)/sizeof(encoded[0]); i++) {
    printbin(encoded[i]);
    printf("\n");
  }
  printf("\n");
  printf("\n");

  unsigned long unencoded[pckcodes];
  
  for (int i = 0; i < pckcodes; i++) {
    int min = pow(2,codelen-1);
    int imin = 0;
    for (int k = 0; k < pow(2,codelen); k++) {
      int hd = hammingdistance(encoded[i],had[k]);
      //printf("\nhd = %d",hd);
      if (hd < min) {
        min = hd;
        imin = k;
      }
    }
    unencoded[i] = 0;
    unencoded[i] = imin;
    printf("hamdist: %d\n",min);
  }

  for (int i = 0; i < sizeof(unencoded)/sizeof(unencoded[0]); i++) {
    printbin(unencoded[i]);
    printf("\n");
  }
  printf("\n");
  printf("\n");

  struct {
    unsigned char bit : 1;
  } bits[sizeof(struct Packet)*CHAR_BIT];

  for (int i = 0; i < pckcodes; i++) {
      for (int j = codelen-1; j >= 0 ; j--) {
        bits[codelen*i+(codelen-(j+1))].bit = (unencoded[i]>>j)&1;
      }
  }

  for (int i = 0; i < sizeof(bits)/sizeof(bits[0]); i++) {
    printf("%d",bits[i].bit);
  }
  printf("\n");
  printf("\n");

  for (int i = 0; i < sizeof(received.values)/sizeof(received.values[0]); i++) {
    for (int j = CHAR_BIT-1; j >= 0; j--) {
      received.values[i] <<= 1;
      received.values[i] += bits[i*CHAR_BIT+j].bit;
    }
  }

  return received.packet;
}


int hammingdistance(unsigned long a, unsigned long b) {
  unsigned long z = a^b;
  int n = 0;
  while (z) {
    if (z&1){
      n++;
    }
    z >>= 1;
  }
  return n;
}

unsigned long *hadamard(int codelen) {
  gsl_matrix *initTop = gsl_matrix_alloc(2,2);
  initTop->tda = 2;
  gsl_matrix_set(initTop,0,0,1); //start of top half with matrix:
  gsl_matrix_set(initTop,0,1,1); // 1  1
  gsl_matrix_set(initTop,1,0,1); // 1 -1
  gsl_matrix_set(initTop,1,1,-1);

  gsl_matrix *matrixTop = gsl_matrix_alloc(1,1);
  matrixTop->tda = 1;
  gsl_matrix_set(matrixTop,0,0,1);

  for (int i = 0; i < codelen-1; i++) { //codelen is -1 as t&b allow *2 the codes
    matrixTop = KPro(matrixTop,initTop);
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

  for (int i = 0; i < codelen-1; i++) {
    matrixBottom = KPro(matrixBottom,initBottom);
  }

  unsigned long *hadamard  = (unsigned long*) malloc(sizeof(unsigned long)*pow(2,codelen));
  double value;

  for (int i = 0; i < pow(2,codelen); i++) {
    *(hadamard+i) = 0;
    for (int j = 0; j < pow(2,codelen-1); j++) {
      *(hadamard+i) <<= 1;
      if (i < pow(2,codelen-1)) {
        value = gsl_matrix_get(matrixTop,i,j);
      } else {
        value = gsl_matrix_get(matrixBottom,i-pow(2,codelen-1),j);
      }
      if (value == 1) {
        *(hadamard+i) += 1;
        printf("1");
      } else {
        printf("0");
      }
    }
    //printf(" %lu ",*(hadamard+i));
    printf("\n");
  }
  printf("\n");
  printf("\n");

  unsigned long *p;
  p = hadamard;

  return p;
}

/*sourced from https://stackoverflow.com/questions/13722543/efficient-way-to-compute-kronecker-product-of-matrices-with-gsl*/

gsl_matrix *KPro(gsl_matrix *a, gsl_matrix *b) {
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

int printbin (long unsigned dec) {
  for (int i = (CHAR_BIT*sizeof(long unsigned))-1; i >= 0; i--) {
    if (((dec>>i)&1)==1) {
      printf("1");
    } else {
      printf("0");
    }
  }
  return 0;
}
