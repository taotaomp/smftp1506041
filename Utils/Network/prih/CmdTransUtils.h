#ifndef __CMDTRANS__
#define __CMDTRANS__ 

extern int sendCmd(int socket,char* cmd_container);
extern int recvMessage(int socket,char* message_container,int flag);
extern int recvMessageWithIOReuse(int socket,char* message_container);

#endif
