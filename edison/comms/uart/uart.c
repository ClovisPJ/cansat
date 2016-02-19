#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "uart.h"

int uart_sendMessage(char *data, int len) {

  int uart;
  uart = open(uart_address, O_WRONLY);
  char *header = "$ZYSK";
  int header_len = 5;

  char *buffer = malloc(len+header_len);
  memcpy(buffer, header, header_len);
  memcpy(buffer+header_len, data, len);
  write(uart, buffer, len+header_len);

  close(uart);
  return EXIT_SUCCESS;
}

char *uart_receiveMessage(int len) {

  int uart;
  uart = open(uart_address, O_RDONLY);
  char *header = "ZYSK";
  int header_len = 4;
  char *data = malloc(len);

  char *buffer = malloc(1);
  while (1) {
    read(uart, buffer, 1);
    if (*buffer == (int)'$') {
      buffer = malloc(len+header_len);
      read(uart, buffer, len+header_len);
      if (memcmp(buffer, header, header_len) == 0) {
        memcpy(data, buffer+header_len, len); 
        break;
      }
      //usleep(500000);
    }
  }

  close(uart);
  return data;
}
