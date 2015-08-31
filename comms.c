#include <stdio.h>
#include <time.h>

int sendMessage();
int receiveMessage();
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

struct Packet {
  struct tm time;
  int type;
  int data[3];
};

union Changeform {
  struct Packet packet;
  char values[sizeof(struct Packet)/sizeof(char)];
};

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

int main() {

  clock_t rawtime = time(NULL);
  int type = 1;
  //int input[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int input[] = {-1,2,-3};

  struct Packet pck;
  pck.time = *localtime(&rawtime);
  pck.type = type;

  int len = sizeof(input)/sizeof(input[0]);
  //pck.data[len];
  for (int i = 0; i < len; i++) {
    pck.data[i] = input[i];
  }

  sendMessage(pck);

  return 0;
}

int sendMessage(struct Packet pck) {

  union Changeform toSend;
  toSend.packet = pck;

/*  for(int o = 0; o < sizeof(struct Packet)/sizeof(char); o++) {
    toSend.values[o] = 255;
  }*/

/*  struct tm *timeinfo;
  timeinfo = &toSend.packet.time;
  printf("%s",asctime(timeinfo)); */

  for (int i = 0; i < sizeof(struct Packet)/sizeof(char); i++) {
    for (int j = 0; j < sizeof(char)*8; j++) {
      printf("%d",(toSend.values[i]>>(7-j))&1);
    }
    //printf("\n");

    char a = toSend.values[i];
    printf("    %d %d",(a>>4)&15,a&15);
    
    //int toSend = hamsqa[a-1];
    printf("\n");
  }
  return 0;
}

int recieveMessage() {
  int input = 0b1111111100000000;
  //example input from signal

  int min = 16;
  int imin = 0;
  for (int i = 0; i < 15; i++) {
    int hd = hammingdistance(input,hamsqa[i]);
    if (hd < min) {
      min = hd;
      imin = i;
    }
  }
  int code = hamsqa[imin];
  int hamdist = min;

  printf("Hamming Distance is %d\n",hamdist);
  printf("and it should be: ");
  for(int k = 0; k < 16; k++) {
    printf("%d",(code>>k)&1);
  }
  printf("\n");

  return 0;
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
