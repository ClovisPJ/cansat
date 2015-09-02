#include <stdio.h>
#include <time.h> 
#include <stddef.h>
#include <gsl/gsl_matrix.h>
#include <math.h>

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
  char values[sizeof(struct Packet)/sizeof(char)];
};

int sendMessage(struct Packet,int);
struct Packet receiveMessage(int);
int hammingdistance(unsigned long, unsigned long);
unsigned long *hadamardsquare(int);
gsl_matrix *KPro(gsl_matrix *, gsl_matrix *);

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

  //sendMessage(pck,256);
  //receiveMessage(256);

  //sendMessage(receiveMessage(256),256);

  return 0;
}

int sendMessage(struct Packet pck, int codelen) {

  union Changeform toSend;
  toSend.packet = pck;

/*  for(int o = 0; o < sizeof(struct Packet)/sizeof(char); o++) {
    toSend.values[o] = 255;
  }*/

/*  struct tm *timeinfo;
  timeinfo = &toSend.packet.time;
  printf("%s",asctime(timeinfo)); */

  if ((codelen <= 0) || !((codelen & (~codelen +1)) == codelen)) {
    return -1; 
    exit(0);
  }

  unsigned long *hadsqa;
  hadsqa = hadamardsquare(codelen);

  for (int i = 0; i < sizeof(struct Packet)/(log2(codelen)/8); i++) {

    printf("value to be sent: ");
    for (int j = log2(codelen)-1; j >= 0; j--) {
      printf("%d",(toSend.values[i]>>j)&1);
    }
    printf("   %d",toSend.values[i]);

    //fake errors
    unsigned long input = hadsqa[toSend.values[i]];
    unsigned long error = 0b0;
    input ^= error;

    printf("\ncode to be sent: ");
    for(int k = codelen-1; k >= 0; k--) {
       printf("%lu",(input>>k)&1);
    }
    printf("    %lu",input);

    //transmit input
    
    printf("\n\n");
  }

  /*for (int i = 0; i < codelen;  i++) {
    for (int j = codelen; j > 0; j--) {
      printf("%lu",(hadsqa[i] >> j)&1);
    }

    printf("    %lu",hadsqa[i]);

    //int toSend = hamsqa[a-1];
    printf("\n");
  }*/

  return 0;
}

struct Packet receiveMessage(int codelen) {
  if ((codelen <= 0) || !((codelen & (~codelen +1)) == codelen)) {
    exit(0);
  }

  unsigned long *hadsqa;
  /*for (int i = 0; i < codelen;  i++) {
    for (int j = codelen; j > 0; j--) {
      printf("%lu",(hadsqa[i] >> j)&1);
    }

    printf("    %lu",hadsqa[i]);

    //int toSend = hamsqa[a-1];
    printf("\n");
  }*/

  hadsqa = hadamardsquare(codelen);

  union Changeform toSend;

  for (int i = 0; i < sizeof(struct Packet)/(log2(codelen)/8); i++) {

    //input from transmission
    unsigned long input = hadsqa[3]; //fake value
    unsigned long error = 0b00000000001000000000000000000000;
    input ^= error;
    
    int min = codelen;
    int imin = 0;
    for (int j = 0; j < codelen; j++) {
      int hd = hammingdistance(input,hadsqa[j]);
      if (hd < min) {
        min = hd;
        imin = j;
      }
    }
    unsigned long code = hadsqa[imin];
    int hamdist = min;

    printf("Hamming Distance is %d\n",hamdist);
    /*printf("code:      ");
    for(int k = codelen; k > 0; k--) {
      printf("%lu",(input>>k)&1);
    }
    printf("    %lu",input);*/

    printf("should be: ");
    for(int k = codelen-1; k >= 0; k--) {
      printf("%lu",(code>>k)&1);
    }
    printf("    %lu",code);

    printf("\nvalue received is: ");
    for(int k = log2(codelen)-1; k >= 0; k--) {
      printf("%d",(imin>>k)&1);
    }
    printf("    %d",imin);

    toSend.values[i] = imin;

    printf("\n\n");
  }
  return toSend.packet;
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

unsigned long *hadamardsquare(int codelen) {
  gsl_matrix *init = gsl_matrix_alloc(2,2);
  init->tda = 2;
  gsl_matrix_set(init,0,0,1);
  gsl_matrix_set(init,0,1,1);
  gsl_matrix_set(init,1,0,1);
  gsl_matrix_set(init,1,1,-1);

  gsl_matrix *matrixhadsqa = gsl_matrix_alloc(1,1);
  matrixhadsqa->tda = 1;
  gsl_matrix_set(matrixhadsqa,0,0,1);

  int rep = log2(codelen);

  for (int i = 1; i <= rep; i++) {
    matrixhadsqa = KPro(matrixhadsqa,init);
  }

  unsigned long hadsqa[codelen];

  for (int i = 0; i < codelen; i++) {
    for (int j = 0; j < codelen; j++) {
      double value = gsl_matrix_get(matrixhadsqa,i,j);
      if (value == 1) {
        hadsqa[i]++;
      }
      hadsqa[i] <<= 1;
    }
  }

  unsigned long *p;
  p = hadsqa;

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
