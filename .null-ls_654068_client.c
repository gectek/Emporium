#include "sockutil.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
int main(void) {

  int clientfd =
      CreateTCPIPv4(); // creating a socket and saving the file descriptor
  char *ip = "127.0.0.1";
  struct sockaddr_in *address;
  address = CreateIPv4Address(ip, 2000);
  int result = connect(clientfd, (struct sockaddr *)address, sizeof(*address));
  if (result == 0) {
    printf("The connection was succeeded!\n"); // printing the output of result
  }
  char *line = NULL;
  size_t lineSize = 0;
  printf("Type and we will send(type exit)..\n");
  while (true) {
    ssize_t charCount = getline(&line, &lineSize, stdin);
    if (charCount > 0) {
      if (strcmp(line, "exit") == 0) {
        break;
      }
      ssize_t amountWasSent = send(clientfd, line, charCount, 0);
      if (amountWasSent == 0)
        printf("Sent successfully\n");
    }
  }

  char buffer[1024];
  recv(clientfd, (void *)buffer, 1024, 0);

  printf("The response was:%s\n", buffer);
  return 0;
}
