#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>		//MSG_WAITALL
#include <stdlib.h>		//system()
#include "../../Utils/Network/prih/TransmissionUtils.h"
#include "../../Utils/Network/prih/CmdTransUtils.h"
#include "../../Utils/Network/prih/FileTransUtils.h"
#include "../../Utils/User/prih/UserCtrlUtils.h"
#include "../../Utils/File/prih/FileCtrlUtils.h"

//函数声明
int processCmd(char* resultContainer,char* cmdValue);
int processPULL(int socket,char* fileName);
int processPUSH(int socket,char* fileName);
/*************************
侦听命令函数
socket:套接字文件描述符
*************************/
void listenCmd(int socket){
	char sendTLVValueContainer[256];					//待发送TLV数据容器
	char recvTLVValueContainer[256];					//接收TLV数据容器
	char *unpacketTLVContainer[3];						//TLV拆包数据容器
	char cmdExecResultContainer[200];					//命令执行结果容器
	int err;

	while(1){
		recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);
		//recvMessageWithIOReuse(socket,recvTLVValueContainer);
		unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);

		if(0 == strcmp("CMD",unpacketTLVContainer[0])){			//判断TLV的Type头是否为CMD
			err = processCmd(cmdExecResultContainer,unpacketTLVContainer[2]);		//调用命令处理函数
			if(0 == err){
				packetTLV(sendTLVValueContainer,"TRUE",strlen(cmdExecResultContainer),cmdExecResultContainer);
				sendCmd(socket,sendTLVValueContainer);		
			}else{
				packetTLV(sendTLVValueContainer,"FALSE",strlen("CMD_exec_error"),"CMD_exec_error");
				sendCmd(socket,sendTLVValueContainer);
			}
		}else if(0 == strcmp("PULL",unpacketTLVContainer[0])){	//判断TLV的Type头是否为PULL
			printf("接收到PULL数据\n");
			processPULL(socket,unpacketTLVContainer[2]);

		}else if(0 == strcmp("PUSH",unpacketTLVContainer[0])){	//判断TLV的Type头是否为PUSH
			printf("接收到PUSH数据\n");
			processPUSH(socket,unpacketTLVContainer[2]);
		}
	}
	
}

/***********************
CMD命令处理函数
resultContainer:命令执行结果的容器
cmdValue:命令值
返回值:命令是否执行成功（0/-1）
************************/
int processCmd(char* resultContainer,char* cmdValue){
	char shellCmdContainer[256];	//shell命令容器
	int fdTempShellResult;			//文件描述符，用于指向一个临时文件
	char *file_container[1024];		//文件容器
	int readLine;					//用于存放读取的文件的行数
	int i;
	int err;

	shellCmdContainer[0] = '\0';		//初始化容器

	fdTempShellResult = createFile(".CmdTemp");		//创建一个临时文件，存储shell命令的返回信息
	closeFile(fdTempShellResult);					//关闭文件

	strcat(shellCmdContainer,cmdValue);				//使用字符串连接函数，将得到的命令后面附加上“ >> .CmdTemp”
	strcat(shellCmdContainer," >> .CmdTemp");		//使其结果存在刚刚创建的临时文件中

	system(shellCmdContainer);						//执行命令
	if(0 > err){
		perror("system");
		system("rm .CmdTemp");
		return -1;
	}

	fdTempShellResult = openFile(".CmdTemp");				//打开临时文件
	readLine = readFile(fdTempShellResult,file_container);	//读取文件内容，并返回读取的行数

	resultContainer[0] = '\0';			//初始化结果容器
	for (i = 0; i <= readLine; i++)		//将读取到的结果写入到结果容器中
	{
		strcat(resultContainer,file_container[i]);
	}
	closeFile(fdTempShellResult);		//关闭文件

	system("rm .CmdTemp");
	return 0;
}

/*********************************
PULL命令处理函数
socket:套接字文件描述符
fileName:文件名
返回值:
*********************************/
int processPULL(int socket,char* fileName){
	char sendTLVValueContainer[256];					//待发送TLV数据容器
	char pullRecvTLVValueContainer[256];					//接收TLV数据容器
	char *unpacketTLVContainer[3];

	char *file_container[1024];
	int fileLine_raw;
	char fileLine[10];
	int fd;

	fd = openFile(fileName);
	if(-1 != fd){										//文件存在并被打开
		fileLine_raw = readFile(fd,file_container);
		sprintf(fileLine,"%d",fileLine_raw);			//将整型转化为字符串
		packetTLV(sendTLVValueContainer,"TRUE",strlen(fileLine),fileLine);	//打包给客户端的回应信息
		sendCmd(socket,sendTLVValueContainer);			//发送回应信息
		
		//recvMessage(socket,pullRecvTLVValueContainer,MSG_WAITALL);	//阻塞接收客户端的ACK回应
		recvMessageWithIOReuse(socket,pullRecvTLVValueContainer);		//接收客户端的ACK回应
		unpacketTLV(pullRecvTLVValueContainer,unpacketTLVContainer);	//解包

		if(0 == strcmp("ACK",unpacketTLVContainer[0])){
			sendFile(socket,file_container,fileLine_raw);			//发送文件
		}


	}else{												//文件打开失败
		packetTLV(sendTLVValueContainer,"FALSE",strlen("File_not_exist"),"File_not_exist");
		sendCmd(socket,sendTLVValueContainer);
	}
	
	
}

/*************************************
PUSH命令处理函数
*************************************/
int processPUSH(int socket,char* fileName){
	char sendTLVValueContainer[256];					//待发送TLV数据容器
	char recvTLVValueContainer[256];					//接收TLV数据容器
	char *unpacketTLVContainer[3];

	int fd;
	int fileLineReal = 0;

	fd = openFile(fileName);
	if(-1 == fd){							//文件不存在
		packetTLV(sendTLVValueContainer,"TRUE",strlen("You_can_upload"),"You_can_upload");
		sendCmd(socket,sendTLVValueContainer);	
	}else{									//文件存在
		packetTLV(sendTLVValueContainer,"FALSE",strlen("File_has_exist"),"File_has_exist");
		sendCmd(socket,sendTLVValueContainer);
		closeFile(fd);
	}

	recvMessageWithIOReuse(socket,recvTLVValueContainer);
	unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);					//解包TLV信息

	if(0 == strcmp("ACK",unpacketTLVContainer[0])){
		if(0 == strcmp("CANCEL",unpacketTLVContainer[2])){						//客户端取消发送
			packetTLV(sendTLVValueContainer,"ACK",strlen("CANCELED"),"CANCELED");
			sendCmd(socket,sendTLVValueContainer);
			return 1;
		}else{
			//获取文件行数，并转换为int
			sscanf(unpacketTLVContainer[2],"%d",&fileLineReal);

			if(fd != -1){	//文件存在
				system("rm -f fileName");	//删除文件
			}
			fd = createFile(fileName);		//创建文件

			packetTLV(sendTLVValueContainer,"ACK",strlen("Upload_pls"),"Upload_pls");
			sendCmd(socket,sendTLVValueContainer);

			receiveFile(socket,fd,fileLineReal);

			closeFile(fd);

			return 0;
		}
	}
	return -1;
}