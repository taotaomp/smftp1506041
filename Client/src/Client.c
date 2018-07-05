#include <sys/socket.h>
#include <stdlib.h>			//system
#include <netinet/in.h>		//struct sockaddr_in
#include <unistd.h>
#include <string.h>
#include "../prih/UserLogin.h"
#include "../prih/CmdCommit.h"
#include "../../Utils/Network/prih/InitUtils.h"
#include "../../Utils/Other/prih/StandardMacro.h"
#include "../../Utils/File/prih/FileCtrlUtils.h"
#include <stdio.h>

int main(int args,int **argv){
	int socketClient;					//客户端套接字文件描述符
	struct sockaddr_in sockaddrClient;	//套接字数据结构
	
	char UserInputContainer[256];		//用户输入命令容器
	char UserInputHeadContainer[6];		//用户输入命令头部容器
	char *UserInputBodyContainer[4];	//用户输入命令拆分容器
	int UserInputSize;					//用户输入命令长度
	int firstSpaceFlag;					//用户输入第一个空格的位置容器

	char encryptKey[8];					//秘钥容器
	int fd_key;							//秘钥文件描述符

	int i;
	int err;

	//初始化套接字
	socketClient = initSocket();
	initSockaddr(&sockaddrClient,STANDARD_IP,STANDARD_PORT);

	//连接服务器
	connectClient(socketClient,&sockaddrClient);
	
	//阻塞接收服务器传来的秘钥信息
	recv(socketClient,encryptKey,8,MSG_WAITALL);
	printf("得到秘钥\n");
	//将秘钥存储在.key文件中
	fd_key = createFile(".key");
	writeFileForSingleLine(fd_key,encryptKey,8);
	close(fd_key);

	//开始用户登录引导
	err = loginGuide(socketClient);
	if(0 == err){			//0则完成登录
		//开始循环接收用户命令
		while(1){
			//用户输入命令，去掉最后一位回车
			UserInputSize = read(1,UserInputContainer,256);
			UserInputContainer[UserInputSize-1] = '\0';
			UserInputSize--;

			//计算用户命令中第一个空格的位置
			for(i = 0 ; i < UserInputSize ; i++){
				if(UserInputContainer[i] == ' '){
					firstSpaceFlag = i;
					break;
				}
			}
			//将空格以前的字符串，放进用户输入命令头部容器中；如果没有输入空格，则将整个命令放入容器中
			if(firstSpaceFlag != 0){
				strncpy(UserInputHeadContainer,UserInputContainer,firstSpaceFlag);
			}else{
				strcpy(UserInputHeadContainer,UserInputContainer);
			}

			//以空格拆分用户命令
			UserInputBodyContainer[0] = strtok(UserInputContainer," ");
			UserInputBodyContainer[1] = strtok(NULL," ");
			UserInputBodyContainer[2] = strtok(NULL," ");
			UserInputBodyContainer[3] = strtok(NULL," ");

			//判断用户输入命令头部容器，根据其中的命令执行函数
			if(0 == strcmp("PULL",UserInputHeadContainer)){
				pullFileCmd(socketClient,UserInputBodyContainer[1],UserInputBodyContainer[2]);

			}else if(0 == strcmp("PUSH",UserInputHeadContainer)){
				pushFileCmd(socketClient,UserInputBodyContainer[1],UserInputBodyContainer[2]);

			}else if(0 == strcmp("REST",UserInputHeadContainer)){
				restFileCmd(socketClient,UserInputBodyContainer[1],UserInputBodyContainer[2]);

			}else if(0 == strcmp("BYE",UserInputHeadContainer)){
				endServiceCmd(socketClient);

				close(socketClient);
				system("rm -f .key");
				break;
			}else if(0 == strcmp("ls",UserInputHeadContainer)||0 == strcmp("pwd",UserInputHeadContainer)||0 == strcmp("cd",UserInputHeadContainer)){
				commitCmd(socketClient,UserInputHeadContainer);
			}else{
				printf("请输入正确命令	\n");
			}
		}
	}
	
}