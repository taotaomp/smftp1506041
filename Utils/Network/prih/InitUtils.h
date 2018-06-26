#ifndef __INIT__
#define __INIT__ 

extern int initSocket();
extern void initSockaddr(struct sockaddr_in *sockaddr_temp,char* ipaddr,int portnum);
extern int bindSocket(int socket,struct sockaddr_in *sockaddr_temp);
extern int listenServer(int socket,int max_row);
extern int acceptServer(int socket,struct sockaddr_in *sockaddr_client,int *sock_length);
extern int connectClient(int socket,struct sockaddr_in *sockaddr_temp);

#endif

