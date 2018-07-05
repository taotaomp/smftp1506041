#include <sys/socket.h>
#include <netinet/in.h>		//struct sockaddr_in
#include <sys/types.h>
#include <unistd.h>
#include "../prih/UserLoginProcess.h"
#include "../prih/CmdProcess.h"
#include "../../Utils/Network/prih/InitUtils.h"
#include "../../Utils/Other/prih/StandardMacro.h"

/*****************************
主函数
*****************************/
int main(int args,char *argv[]){
	int socketServer,socketClient;
	struct sockaddr_in sockaddrServer,sockaddrClient;
	int sock_length;
	pid_t pid;

	socketServer = initSocket();
	initSockaddr(&sockaddrServer,STANDARD_IP,STANDARD_PORT);
	bindSocket(socketServer,&sockaddrServer);

	listenServer(socketServer,STANDARD_LISTEN_LENGTH);

	while(1){
		socketClient = acceptServer(socketServer,&sockaddrServer,&sock_length);
		pid = fork();
			if(0 == pid){	//子进程内
				close(socketServer);	//关闭侦听套接字
				if(0 == processLogin(socketClient)){
					listenCmd(socketClient);
				}
			}else{		//父进程
				close(socketClient);	//关闭客户端套接字
				//printf("服务器继续侦听\n");	
			}
		
	}
}
