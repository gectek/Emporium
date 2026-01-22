#include "sockutil.h"
#include <bits/pthreadtypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
void *Recv_and_print_Msg(void *SocketFD){
  char buffer[1024];
  int FD = *((int*)SocketFD);  
    
  while (true) {    
    ssize_t amountrecv = recv(FD, (void *)buffer, 1024,0);    
    if (amountrecv > 0) {
      buffer[amountrecv] = 0;      
      printf("%s",buffer);      
    }
    if (amountrecv == 0) {
      printf("Closing the socket...\n");      
      break;
    }
    if(amountrecv<0){
        perror("recv");
        break;    
    }    
  }
  free(SocketFD);  
  close(FD);
  return 0;  
}

void recvUsingNewThread(int SocketFD){
    pthread_t id;
    int *FD = malloc(sizeof(int));
    *FD = SocketFD;
    pthread_create(&id,NULL,Recv_and_print_Msg,FD);
}
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
  recvUsingNewThread(clientfd);
  char *name = NULL;
  size_t nameSize = 0;
  printf("Enter your username:");
  ssize_t nameCount = getline(&name,&nameSize,stdin);  
  ssize_t nameSend = send(clientfd,name,nameCount,0);
  if(nameSend>0)
        printf("You are registered as %s\n",name);
  char *line = NULL;
  size_t lineSize = 0;
  printf("Type and we will send(type exit)..\n");
  while (true) {    
    ssize_t charCount = getline(&line, &lineSize, stdin);
    if (charCount > 0) {
      if (strcmp(line, "exit") == 0 || strcmp(line,"exit\n")==0){
        printf("closing....\n");        
        break;}
      ssize_t amountWasSent = send(clientfd, line, charCount, 0);
      if (amountWasSent > 0) {
        printf("Sent successfully\n");
      }
    }
  }

  close(clientfd);

  return 0;
}
