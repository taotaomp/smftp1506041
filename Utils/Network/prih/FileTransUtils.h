#ifndef __FILETRANS__
#define __FILETRANS__

extern void sendFile(int sfd,socketaddr_store *sockaddr_to,char *file_container[],int line_count);
extern void receiveFile(int sfd,socketaddr_store *sockaddr_from,char *file_container[],int line_count);

#endif