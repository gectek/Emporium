#ifndef SOCKUTILS_H
#define SOCKUTILS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>

struct AcceptedSocket{
    int AcceptedClientFD;
    char name[20];
    int namesize;
    struct sockaddr_in address;
    int error;
    bool AcceptedSuccess;
};
int CreateTCPIPv4();
struct sockaddr_in* CreateIPv4Address(char *ip,int port);

struct AcceptedSocket *acceptConnection(int serverfd);

#endif
