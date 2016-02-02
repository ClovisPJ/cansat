#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int send() {
  int uart;
  uart = open("/dev/ttyACM0", O_WRONLY);

  char buffer[] = "Hello World!";
  write(uart, buffer, 13);

  close(uart);
  return 0;
}

int receive() {
  int uart;
  uart = open("/dev/ttyACM0", O_RDONLY);

  char *buffer = malloc(13);
  read(uart, buffer, 13);

  close(uart);
  puts(buffer);
  return 0;
}

int main() {

  send();
  receive();
  return 0;
}
