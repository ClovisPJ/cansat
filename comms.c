#include <stdio.h>
#include <time.h>

int matchcode();
int hammingdistance();

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

union changeform {
  struct tm time;
  int msg;
};

int main() {
  int x = 0b1011110010011101;
  printf("this should be: %d\n",matchcode(x));

  union changeform msg;
  time_t rawtime;
  struct tm *timeinfo;

  rawtime = time(NULL);
  msg.time = *localtime( &rawtime);

  timeinfo = &msg.time;
  printf("%s",asctime(timeinfo));

  return 0;
}

int matchcode(int a) {
  int hamsqa[16] =
    {0b1111111111111111,
     0b1010101010101010,
     0b1010101011001100,
     0b1001100110011001,
     0b1111000011110000,
     0b1010010110100101,
     0b1100001111000011,
     0b1001011010010110,
     0b1111111100000000,
     0b1010101001010101,
     0b1100110000110011,
     0b1001100101100110,
     0b1111000000001111,
     0b1010010101011010,
     0b1100001100111100,
     0b1001011001101001};
  int min = 16;
  int imin = 0;
  int i = 0;
  for (i < 16; i++;) {
    int hd = hammingdistance(a,hamsqa[i]);
    if (hd < min) {
      min = hd;
      imin = i;
    }
  }
  return hamsqa[imin];
}

int hammingdistance(int a, int b) {
  int z = a^b;
  int n = 0;
  while (z) {
    if (z&1){
      n++;
    }
    z >>= 1;
  }
  return n;
}
