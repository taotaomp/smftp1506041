#ifndef __CMDCOMMIT__
#define __CMDCOMMIT__

extern void commitCmd(int socket,char* cmd_container);
extern int pullFileCmd(int socket,char* file_name_container_in_Server,char *file_name_container_in_Client);
extern int pushFileCmd(int socket,char* file_name_container_in_Client,char *file_name_container_in_Server);

#endif