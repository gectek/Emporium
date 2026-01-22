#include "sockutil.h"
struct AcceptedSocket *acceptConnection(int serverfd){
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketfd = accept(serverfd,(struct sockaddr*)&clientAddress,(socklen_t*)&clientAddressSize);
    struct AcceptedSocket *NewClientSocket = malloc(sizeof(struct AcceptedSocket));
    NewClientSocket->AcceptedClientFD = clientSocketfd;
    NewClientSocket->address = clientAddress;
    NewClientSocket->AcceptedSuccess = clientSocketfd>0;
    if(!NewClientSocket->AcceptedSuccess)
        NewClientSocket->error = clientSocketfd;
    return NewClientSocket;
    }

int CreateTCPIPv4(){
    int fd = socket(AF_INET,SOCK_STREAM,0);
    return fd;
}
struct sockaddr_in* CreateIPv4Address(char *ip,int port){
  struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
  address->sin_port = htons(port);
  address->sin_family = AF_INET;
  
  if(strlen(ip) == 0){
        address->sin_addr.s_addr = INADDR_ANY;
  }
  else{  
        inet_pton(AF_INET, ip,&address->sin_addr.s_addr); // taking the address and saving it into a buffer
  } 
  return address; 
}

