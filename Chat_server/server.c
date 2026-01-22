#include "sockutil.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX_CLIENTS 10
int client_count = 0;
struct AcceptedSocket *Clients[MAX_CLIENTS];
void DistributionToNewThread(struct AcceptedSocket *Client);
void AcceptAndRecv_PrintData(int serverSocketFD);
void Broadcast(char buffer[],int SocketFD);
void *Recv_and_print_Msg(void *SocketFD){
  char buffer[1024];
  int FD = *((int*)SocketFD);  
  int i=0;
  for(i=0;i<client_count;i++)
    {
        if(FD == Clients[i]->AcceptedClientFD)
        {
            break;
        }
    }
  while (true) {
    ssize_t amountrecv = recv(FD, (void *)buffer, 1024, 0);
    if (amountrecv > 0) {
      buffer[amountrecv] = 0;      
      printf("%s : %s",Clients[i]->name,buffer);
      Broadcast(buffer,FD);      
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
void Handle_clients(int serverSocketFD){
  char Name[20];
  int name_leng;  
  while(true){
    printf("Waiting for clients...\n");    
    struct AcceptedSocket *Client = acceptConnection(serverSocketFD);
    printf("Accepted a client!!\n");
    ssize_t namerecv = recv(Client->AcceptedClientFD,(void*)Name,20,0);
    if(namerecv < 0){
            perror("recv");
            break;
        }
    name_leng = strlen(Name);    
    Name[name_leng-1] = '\0';        
    strncpy(Client->name,Name,strlen(Name));
    Client->namesize = strlen(Name);
    //Have to implement a queue here....
    Clients[client_count++] = Client;    
    DistributionToNewThread(Client);    
    }
}
void Broadcast(char buffer[],int SocketFD){
    char packet[1024];
    char Name[20];
    int i=0;
    for(int j = 0;j<client_count;j++){       
        if(Clients[j]->AcceptedClientFD == SocketFD){
             strncpy(Name,Clients[j]->name, Clients[j]->namesize);
        }
        printf("Name is : %s\n",Name);
        if(Clients[j]->AcceptedClientFD != SocketFD){
            snprintf(packet,sizeof(packet),"%s : %s",Name,buffer);
            
            send(Clients[j]->AcceptedClientFD,packet,strlen(packet),0);
        }
    }

}
void DistributionToNewThread(struct AcceptedSocket *Client){
    pthread_t id;
    int *FD = malloc(sizeof(int));
    *FD = Client->AcceptedClientFD;
    printf("Creating a new thread\n");
    pthread_create(&id,NULL,Recv_and_print_Msg,FD);
}

int main() {
  int serverfd = CreateTCPIPv4();
  struct sockaddr_in *server_address = CreateIPv4Address("", 2000);

  int result_b = bind(serverfd, (struct sockaddr *)server_address,
                      sizeof(*server_address));
  if (result_b == 0) {
    printf("The Socket is bound succesfully...\n");
  }

  int result_l = listen(serverfd, 10);
  if (result_l == 0) {
    printf("The server is Listening....\n");
  }

  Handle_clients(serverfd);

  shutdown(serverfd, SHUT_RDWR);
  return 0;
}
