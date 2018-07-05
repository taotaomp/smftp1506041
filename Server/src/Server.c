#include <sys/socket.h>
#include <netinet/in.h>		//struct sockaddr_in
#include <sys/types.h>
#include <unistd.h>
#include "../prih/UserLoginProcess.h"
#include "../prih/CmdProcess.h"
#include "../../Utils/Network/prih/InitUtils.h"
#include "../../Utils/Other/prih/StandardMacro.h"
#include "../../Utils/Network/prih/TransEncryptUtils.h"
#include "../../Utils/File/prih/FileCtrlUtils.h"
#include "../../Utils/Other/prih/IOMoniterUtils.h"
#include <stdio.h>
#include <signal.h>

void sig_pipe(int sign);
/*****************************
主函数
*****************************/
int main(int args,char *argv[]){
	int socketServer,socketClient;						//服务器和客户端套接字文件描述符
	struct sockaddr_in sockaddrServer,sockaddrClient;	//服务器和客户端套接字数据结构
	int sock_length;									//套接字长度（accept使用）
	
	pid_t pid;
	int err;

	char encryptKey[8];									//秘钥容器
	int key_fd;											//秘钥文件描述符

	signal(SIGPIPE,sig_pipe);
	//初始化套接字,绑定,侦听
	socketServer = initSocket();
	initSockaddr(&sockaddrServer,STANDARD_IP,STANDARD_PORT);
	bindSocket(socketServer,&sockaddrServer);
	listenServer(socketServer,STANDARD_LISTEN_LENGTH);

	//生成秘钥，存储在秘钥文件中
	generateKey(encryptKey);
	key_fd = createFile(".key");
	writeFileForSingleLine(key_fd,encryptKey,8);
	close(key_fd);

	while(1){
		err = selectSfdRead(socketServer);
		if(0 == err){
			continue;
		}else if(err >0){
			socketClient = acceptServer(socketServer,&sockaddrServer,&sock_length);
			printf("已接收请求\n");
		}
		
		//一旦客户端器连接，将秘钥发送给客户端
		write(socketClient,encryptKey,8);

		//判断客户端是否登录成功
		if(0 == processLogin(socketClient)){
			listenCmd(socketClient);
		}else{
			continue;
		}
	}
}

void sig_pipe(int sign){
	printf("失去客户端连接\n");

}
