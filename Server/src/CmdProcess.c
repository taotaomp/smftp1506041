#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>		//MSG_WAITALL
#include <stdlib.h>		//system()
#include "../../Utils/Network/prih/TransmissionUtils.h"
#include "../../Utils/Network/prih/CmdTransUtils.h"
#include "../../Utils/User/prih/UserCtrlUtils.h"
#include "../../Utils/File/prih/FileCtrlUtils.h"

//函数声明
int processCmdLs(char* resultContainer,char* cmdValue);
/*************************
侦听命令函数
*************************/
void listenCmd(int socket){
	char sendTLVValueContainer[256];					//待发送TLV数据容器
	char recvTLVValueContainer[256];					//接收TLV数据容器
	char *unpacketTLVContainer[3];						//TLV拆包数据容器
	char cmdExecResultContainer[200];					//命令执行结果容器
	int err;

	while(1){
		recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);
		unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);

		if(0 == strcmp("CMD",unpacketTLVContainer[0])){			//判断TLV的Type头是否为CMD
			err = processCmd(cmdExecResultContainer,unpacketTLVContainer[2]);		//调用命令处理函数
			if(0 == err){
				packetTLV(sendTLVValueContainer,"TRUE",strlen(cmdExecResultContainer),cmdExecResultContainer);
				sendCmd(socket,sendTLVValueContainer);		
			}else{
				packetTLV(sendTLVValueContainer,"FAlSE",strlen("执行错误"),"执行错误");
				sendCmd(socket,sendTLVValueContainer);
			}
		}else if(0 == strcmp("PULL",unpacketTLVContainer[0])){	//判断TLV的Type头是否为PULL

		}else if(0 == strcmp("PUSH",unpacketTLVContainer[0])){	//判断TLV的Type头是否为PUSH

		}
	}
	
}

/***********************
CMD命令处理函数
resultContainer:命令执行结果的容器
cmdValue:命令值
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
*********************************/
int processPULL(char* fileName){
	char sendTLVValueContainer[256];					//待发送TLV数据容器
	char recvTLVValueContainer[256];					//接收TLV数据容器
	char *file_container[1024];
	int fileLine;
	int fd;

	fd = openFile(fileName);
	if(-1 != fd){
		fileLine = readFile(fd,file_container);
		packetTLV(sendTLVValueContainer,"TRUE",,cmdExecResultContainer);
		//网页数字转字符串
	}else{

	}
	
	packetTLV(sendTLVValueContainer,"TRUE",strlen(cmdExecResultContainer),cmdExecResultContainer);
	sendCmd();
}