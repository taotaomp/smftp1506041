#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>		//MSG_WAITALL
#include "../../Utils/Network/prih/TransmissionUtils.h"
#include "../../Utils/Network/prih/CmdTransUtils.h"
#include "../../Utils/File/prih/FileCtrlUtils.h"
#include "../../Utils/Network/prih/FileTransUtils.h"

/*************************************
提交CMD命令
socket:套接字文件描述符
cmd_container:命令（ls|pwd|cd）
*************************************/
void commitCmd(int socket,char* cmd_container){
	char sendTLVValueContainer[256];		//发送TLV消息容器
	char recvTLVValueContainer[256];		//接收TLV消息容器
	char *unpacketTLVContainer[3];			//拆解TLV消息容器
	int err;

	//打包并发送TLV消息
	packetTLV(sendTLVValueContainer,"CMD",strlen(cmd_container),cmd_container);
	sendCmd(socket,sendTLVValueContainer);	

	//阻塞等待服务器返回的消息，得到消息后拆解
	//recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);						
	err = recvMessageWithIOReuse(socket,recvTLVValueContainer);
	if(0 == err){
		printf("服务器无响应\n");
		return;
	}
	unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);			//解包TLV信息

	//判断TLV消息头部是否为TRUE或FALSE
	if(0 == strcmp(unpacketTLVContainer[0],"TRUE")){
		puts(unpacketTLVContainer[2]);				//输出TLV中的value

	}else if(0 == strcmp(unpacketTLVContainer[0],"FALSE")){
		printf("错误：%s\n",unpacketTLVContainer[2]);

	}else{
		printf("错误：服务器返回了一个错误的回复\n");

	}
	
}

/************************************
拉取文件命令
描述：客户端向服务器发送一条TLV消息：Type为PULL，value为文件的路径
	 如果服务器上文件存在，则服务器返回的TLV消息中，Type为TRUE，value为读取的文件的“行数”
	 如果服务器上文件不存在，则服务器返回的TLV消息中，Type为FALSE
socket:套接字文件描述符
file_name_container_in_Server:服务器上文件的路径
file_name_container_in_Client:客户端上保存文件的路径
************************************/
int pullFileCmd(int socket,char* file_name_container_in_Server,char *file_name_container_in_Client){
	char sendTLVValueContainer[256];			//发送TLV消息容器
	char recvTLVValueContainer[256];			//接收TLV消息容器
	char *unpacketTLVContainer[3];				//拆解TLV消息容器

	int fileLineReal = 0;						//存放转换成int的文件行数
	int fd;
	int err;

	//封装并发送PULL的TLV消息（含文件的路径）
	packetTLV(sendTLVValueContainer,"PULL",strlen(file_name_container_in_Server),file_name_container_in_Server);
	sendCmd(socket,sendTLVValueContainer);										//发送提交或拉取命令

	//阻塞等待服务器返回的消息，得到消息后拆解
	//recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);
	err = recvMessageWithIOReuse(socket,recvTLVValueContainer);
	if(0 == err){	//超时
		printf("服务器无响应\n");
		system("rm -f ");
		return -1;
	}
	unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);					//解包TLV信息

	//判断TLV消息头部是否为TRUE或FALSE
	if(0 == strcmp("TRUE",unpacketTLVContainer[0])){			//拉取文件，成功后TLV的Value字段会有文件的“行数”
		//获取文件行数，并转换为int
		sscanf(unpacketTLVContainer[2],"%d",&fileLineReal);

		//创建本地文件
		fd = createFile(file_name_container_in_Client);
		if(-1 == fd){
			return -1;
		}

		//封装回复服务器的TLV消息并发送
		packetTLV(sendTLVValueContainer,"ACK",strlen("recv_Ready"),"recv_Ready");
		sendCmd(socket,sendTLVValueContainer);	
		
		//开始接收文件
		receiveFile(socket,fd,fileLineReal);
		
		closeFile(fd);
		return 0;
				
	}else if(0 == strcmp("FALSE",unpacketTLVContainer[0])){
		printf("错误：%s\n",unpacketTLVContainer[2]);

		closeFile(fd);
		return -1;
	}else{
		printf("错误：服务器返回了一个错误的回复\n");

		closeFile(fd);
		return -1;
	}
}

/**********************************
推送文件命令
描述:客户端向服务器发送一条TLV消息：Type为PUSH，value为文件路径
	 如果服务器上文件存在，则服务器返回的TLV消息中，Type为FALSE，value为读取的文件的“行数”
	 如果服务器上文件不存在，则服务器返回的TLV消息中，Type为True
**********************************/
int pushFileCmd(int socket,char* file_name_container_in_Client,char *file_name_container_in_Server){
	char sendTLVValueContainer[256];				//发送TLV消息容器
	char recvTLVValueContainer[256];				//接收TLV消息容器
	char *unpacketTLVContainer[3];					//拆解TLV消息容器
	char *file_container[1024];						//读取文件容器
	char userConfirmContainer;						//用户确认字符容器

	int fd;
	int err;
	int fileLine_raw = 0;							//读取的文件行数
	char fileLine[10];								//转换成char的文件行数

	//打开本地文件
	fd = openFile(file_name_container_in_Client);
	if(-1 == fd){
		return -1;
	}

	//读取本地文件到容器中，并获得读取的行数，将行数转换成字符型
	fileLine_raw = readFile(fd,file_container);
	sprintf(fileLine,"%d",fileLine_raw);			//将整型转化为字符串

	//封装并发送PUSH的TLV消息（含文件的路径）
	packetTLV(sendTLVValueContainer,"PUSH",strlen(file_name_container_in_Server),file_name_container_in_Server);
	sendCmd(socket,sendTLVValueContainer);

	//阻塞等待服务器返回的消息，得到消息后拆解
	err = recvMessageWithIOReuse(socket,recvTLVValueContainer);
	if(0 == err){	//超时
		printf("服务器无响应\n");
		return -1;
	}
	unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);					

	//判断TLV消息头部是否为TRUE或FALSE
	if(0 == strcmp("TRUE",unpacketTLVContainer[0])){		//服务器回应TRUE表明服务器不存在文件冲突
		packetTLV(sendTLVValueContainer,"ACK",strlen(fileLine),fileLine);	//打包给服务端的回应信息
		sendCmd(socket,sendTLVValueContainer);				//发送回应信息

	}else if(0 == strcmp("FALSE",unpacketTLVContainer[0])){	//服务器回应FALSE表明服务器上存在同名文件
		printf("服务器上已存在该文件，要覆盖吗?（y/n）");
		scanf("%c",&userConfirmContainer);
		if(userConfirmContainer == 'y'){
			packetTLV(sendTLVValueContainer,"ACK",strlen(fileLine),fileLine);	//打包给服务端的回应信息
		}else if(userConfirmContainer == 'n'){
			packetTLV(sendTLVValueContainer,"ACK",strlen("CANCEL"),"CANCEL");	//打包给服务端的回应信息
		}else{
			printf("错误的命令，取消发送\n");
			packetTLV(sendTLVValueContainer,"ACK",strlen("CANCEL"),"CANCEL");	//打包给服务端的回应信息
		}
		sendCmd(socket,sendTLVValueContainer);			//发送回应信息
	}

	//阻塞等待服务器返回的消息，得到消息后拆解
	err = recvMessageWithIOReuse(socket,recvTLVValueContainer);
	if(0 == err){	//超时
		printf("服务器无响应\n");
		return -1;
	}
	unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);					//解包TLV信息

	//判断TLV消息头部是否为ACK
	if(0 == strcmp("ACK",unpacketTLVContainer[0])){
		if(0 == strcmp("Upload_pls",unpacketTLVContainer[2])){
			sendFile(socket,file_container,fileLine_raw);
			return 0;
		}else{				//服务器收到取消指令
			return -1;
		}

	}else{
			printf("错误：服务器返回了一个错误的回复\n");
			return -1;
	}
}

/********************************
续传文件命令
********************************/
int restFileCmd(int socket,char *file_name_container_in_Server,char *file_name_container_in_Client){
	char sendTLVValueContainer[256];			//发送TLV消息容器
	char recvTLVValueContainer[256];			//接收TLV消息容器
	char *unpacketTLVContainer[3];				//拆解TLV消息容器
	char *file_container[1024];					//读取文件容器

	int fd;
	int fileExistCount = 0;						//文件中已经存在的字节数
	char fileExistCountChar[10];				//转换成char的，文件中已经存在的字节数
	int fileLineReal;							//存放转换成int的文件行数
	char tempCharContainer[1];					//计算文件中已经存在的字节数时，用到的临时容器
	int err;

	//打开本地文件
	fd = openFile(file_name_container_in_Client);
	if(-1 == fd){
		return -1;
	}

	//封装并发送REST的TLV消息（含文件的路径）
	packetTLV(sendTLVValueContainer,"REST",strlen(file_name_container_in_Server),file_name_container_in_Server);
	sendCmd(socket,sendTLVValueContainer);
printf("发送REST完成\n");
	//阻塞等待服务器返回的消息，得到消息后拆解
	err = recvMessageWithIOReuse(socket,recvTLVValueContainer);
	if(0 == err){	//超时
		printf("服务器无响应\n");
		return -1;
	}
	unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);

	//判断TLV消息头部是否为TRUE或FALSE
	if(0 == strcmp("TRUE",unpacketTLVContainer[0])){
		//统计文件中现有的字节的个数
		while(1){
			err = read(fd,tempCharContainer,1);
			if(err == 0){	//EOF
				break;		
			}
			fileExistCount++;
		}
		//将文件字节数转换成字符串，打包成ACK的TLV后，发送
		sprintf(fileExistCountChar,"%d",fileExistCount);
		packetTLV(sendTLVValueContainer,"ACK",strlen(fileExistCountChar),fileExistCountChar);
		sendCmd(socket,sendTLVValueContainer);
		//阻塞等待服务器返回的消息，得到消息后拆解
		err = recvMessageWithIOReuse(socket,recvTLVValueContainer);
		if(0 == err){	//超时
			printf("服务器无响应\n");
			return -1;
		}
		unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);
		//判断TLV消息头部是否为ACK
		if(0 == strcmp("ACK",unpacketTLVContainer[0])){
			//获取文件行数，并转换为int
			sscanf(unpacketTLVContainer[2],"%d",&fileLineReal);
			//开始接收文件
			receiveFile(socket,fd,fileLineReal);
			return 0;

		}else{
			printf("错误：服务器返回了一个错误的回复\n");
			return -1;
		}

	}else{
		printf("错误：%s\n",unpacketTLVContainer[2]);
		return -1;
	}
	
}

/******************************
结束服务命令
******************************/
void endServiceCmd(int socket){
	char sendTLVValueContainer[256];			//发送TLV消息容器

	packetTLV(sendTLVValueContainer,"BYE",strlen("VALUE"),"VALUE");
	sendCmd(socket,sendTLVValueContainer);
}