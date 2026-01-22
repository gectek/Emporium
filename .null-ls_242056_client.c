#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
int CreateTCPIPv4(){
    int fd = socket(AF_INET,SOCK_STREAM,0);
    return fd;
}
struct sockaddr_in* CreateIPv4Address(char *ip,int port){
  struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
  address->sin_port = htons(port);
  address->sin_family = AF_INET;
  inet_pton(AF_INET, ip,&address->sin_addr.s_addr); // taking the address and saving it into a buffer
  return address; 
}
int main(void) {
     
  int clientfd = CreateTCPIPv4();   // creating a socket and saving the file descriptor
  char *ip = "142.251.16.100"; 
  struct sockaddr_in *address;
  address = CreateIPv4Address(ip,80);  
  int result = connect(clientfd, (struct sockaddr *)address,sizeof(*address));
  if (result == 0) {
    printf("The connection was succeeded!\n"); // printing the output of result
  }
  char *message;
  message = "GET \\ HTTP/1.1\r\nHost:google.com\r\n\r\n";
  send(clientfd, (const void*)message, strlen(message), 0);

  char buffer[1024];
  recv(clientfd, (void*)buffer, 1024, 0);

  printf("The response was:%s\n", buffer);
  return 0;
}
