#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>		//MSG_WAITALL
#include <stdlib.h>		//system()
#include <sys/types.h>	//lseek()
#include "../../Utils/Network/prih/TransmissionUtils.h"
#include "../../Utils/Network/prih/CmdTransUtils.h"
#include "../../Utils/Network/prih/FileTransUtils.h"
#include "../../Utils/User/prih/UserCtrlUtils.h"
#include "../../Utils/File/prih/FileCtrlUtils.h"

//函数声明
int processCmd(char* resultContainer,char* cmdValue);
int processPULL(int socket,char* fileName);
int processPUSH(int socket,char* fileName);
int processREST(int socket,char* fileName);

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
	int cycleTime = 0;

	while(1){
		//阻塞等待客户端的请求，接收请求后拆解TLV结构
		err = recvMessageWithIOReuse(socket,recvTLVValueContainer);
		//err = recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);
		if(0 == err){
			cycleTime++;
			if(cycleTime == 100){
				break;
			}
			continue;
		}
		unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);

		if(0 == strcmp("CMD",unpacketTLVContainer[0])){			//判断TLV的Type头是否为CMD
			//调用命令处理函数
			err = processCmd(cmdExecResultContainer,unpacketTLVContainer[2]);
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

		}else if(0 == strcmp("REST",unpacketTLVContainer[0])){	//判断TLV的Type头是否为REST
			printf("接收到REST数据\n");
			processREST(socket,unpacketTLVContainer[2]);

		}else if(0 == strcmp("BYE",unpacketTLVContainer[0])){	//判断TLV的Type头是否为BYE
			printf("用户结束\n");
			break;
		}else{
			packetTLV(sendTLVValueContainer,"FALSE",strlen("Error_request"),"Error_request");
			sendCmd(socket,sendTLVValueContainer);
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

	shellCmdContainer[0] = '\0';	//初始化shell命令容器

	fdTempShellResult = createFile(".CmdTemp");		//创建一个临时文件，用来存储shell命令的返回信息
	closeFile(fdTempShellResult);					//关闭文件

	strcat(shellCmdContainer,cmdValue);				//使用字符串连接函数，将得到的命令后面附加上“ >> .CmdTemp”
	strcat(shellCmdContainer," >> .CmdTemp");		//使其结果存在刚刚创建的临时文件中

	err = system(shellCmdContainer);						//执行命令
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
	char sendTLVValueContainer[256];					//发送TLV消息容器
	char pullRecvTLVValueContainer[256];				//接收TLV消息容器
	char *unpacketTLVContainer[3];						//拆解TLV消息容器

	char *file_container[1024];							//读取文件容器
	int fileLine_raw;									//读取的文件行数
	char fileLine[10];									//转换成char的文件行数
	int fd;
	int err;

	//打开服务器上的文件
	fd = openFile(fileName);
	if(-1 != fd){										//文件存在并被打开
		//读取服务器文件到容器中，并获得读取的行数，将行数转换成字符型
		fileLine_raw = readFile(fd,file_container);
		sprintf(fileLine,"%d",fileLine_raw);			//将整型转化为字符串

		//封装并发送回复的TLV消息（含文件的行数）
		packetTLV(sendTLVValueContainer,"TRUE",strlen(fileLine),fileLine);	
		sendCmd(socket,sendTLVValueContainer);			
		
		//阻塞等待客户端返回的消息，得到消息后拆解
		//recvMessage(socket,pullRecvTLVValueContainer,MSG_WAITALL);	//阻塞接收客户端的ACK回应
		err = recvMessageWithIOReuse(socket,pullRecvTLVValueContainer);		//接收客户端的ACK回应
		if(0 == err){	//超时
			return -1;
		}
		unpacketTLV(pullRecvTLVValueContainer,unpacketTLVContainer);	

		//判断TLV消息头部是否为ACK
		if(0 == strcmp("ACK",unpacketTLVContainer[0])){
			sendFile(socket,file_container,fileLine_raw);			//发送文件
			return 0;
		}else{		//未收到正确消息
			return -1;
		}

	}else{			//文件打开失败
		packetTLV(sendTLVValueContainer,"FALSE",strlen("File_not_exist"),"File_not_exist");
		sendCmd(socket,sendTLVValueContainer);
		return -1;
	}
	
	
}

/*************************************
PUSH命令处理函数
*************************************/
int processPUSH(int socket,char* fileName){
	char sendTLVValueContainer[256];					//发送TLV消息容器
	char recvTLVValueContainer[256];					//接收TLV消息容器
	char *unpacketTLVContainer[3];						//拆解TLV消息容器

	int fd;
	int fileLineReal = 0;								//存放转换成int的文件行数
	int err;

	//尝试打开服务器上的文件
	fd = openFile(fileName);
	if(-1 == fd){							//文件不存在
		packetTLV(sendTLVValueContainer,"TRUE",strlen("You_can_upload"),"You_can_upload");
		sendCmd(socket,sendTLVValueContainer);	
	}else{									//文件存在
		packetTLV(sendTLVValueContainer,"FALSE",strlen("File_has_exist"),"File_has_exist");
		sendCmd(socket,sendTLVValueContainer);
	}

	//阻塞等待服务器返回的消息，得到消息后拆解
	err = recvMessageWithIOReuse(socket,recvTLVValueContainer);
	if(0 == err){	//超时
		return -1;
	}
	unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);					//解包TLV信息

	//判断TLV消息头部是否为ACK
	if(0 == strcmp("ACK",unpacketTLVContainer[0])){
		if(0 == strcmp("CANCEL",unpacketTLVContainer[2])){						//客户端取消发送
			//封装并发送ACK的TLV消息
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

			//封装并发送ACK的TLV消息
			packetTLV(sendTLVValueContainer,"ACK",strlen("Upload_pls"),"Upload_pls");
			sendCmd(socket,sendTLVValueContainer);

			//开始接收文件
			receiveFile(socket,fd,fileLineReal);

			closeFile(fd);

			return 0;
		}
	}else{	//接收到错误回复消息
		return -1;
	}
}

/**************************************
续传命令处理函数
**************************************/
int processREST(int socket,char* fileName){
	char sendTLVValueContainer[256];					//发送TLV消息容器
	char recvTLVValueContainer[256];					//接收TLV消息容器
	char *unpacketTLVContainer[3];						//拆解TLV消息容器
	char *file_container[1024];							//读取文件容器

	int fd;
	int err;
	int offset;											//存放客户端传来的文件偏移量
	off_t offset_result;								//存放文件偏移操作的结果
	int fileLine_raw;									//读取的文件行数
	char* fileLine;										//转换成char的文件行数
	
	//打开服务器上的文件
	fd = openFile(fileName);
	if(-1 != fd){			//文件存在并被打开
		//封装并发送回复的TLV消息
		packetTLV(sendTLVValueContainer,"TRUE",strlen("You_can_download"),"You_can_download");
		sendCmd(socket,sendTLVValueContainer);

		//阻塞等待服务器返回的消息，得到消息后拆解
		err = recvMessageWithIOReuse(socket,recvTLVValueContainer);
		if(0 == err){	//超时
			return -1;
		}
		unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);

		//判断TLV消息头部是否为ACK
		if(0 == strcmp("ACK",unpacketTLVContainer[0])){
			//获取偏移量，并转换为int
			sscanf(unpacketTLVContainer[2],"%d",&offset);

			//对文件进行偏移
			offset_result = lseek(fd,(off_t)offset,SEEK_SET);
			if(-1 != offset_result){		//偏移成功

				//读取文件到容器中，并获得读取的行数，将行数转换成字符型
				fileLine_raw = readFile(fd,file_container);
				sprintf(fileLine,"%d",fileLine_raw);
				
				//封装并发送回复的TLV消息
				packetTLV(sendTLVValueContainer,"ACK",strlen(fileLine),fileLine);
				sendCmd(socket,sendTLVValueContainer);

				//开始发送文件
				sendFile(socket,file_container,fileLine_raw);
			}
		}
	}else{
		packetTLV(sendTLVValueContainer,"FALSE",strlen("File_not_exist"),"File_not_exist");
		sendCmd(socket,sendTLVValueContainer);
		return -1;
	}

}