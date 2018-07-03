#ifndef __FILETRANS__
#define __FILETRANS__

extern void sendFile(int sfd,char *file_container[],int line_count);
extern void receiveFile(int sfd,int fd,int line_count);

#endif