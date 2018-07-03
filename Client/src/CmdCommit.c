#include <stdio.h>
#include <unistd.h>
#include <string.h>
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
	char sendTLVValueContainer[256];
	char recvTLVValueContainer[256];
	char *unpacketTLVContainer[3];
	int err;

	packetTLV(sendTLVValueContainer,"CMD",strlen(cmd_container),cmd_container);
	sendCmd(socket,sendTLVValueContainer);	

	//recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);						//阻塞等待服务器返回的消息
	err = recvMessageWithIOReuse(socket,recvTLVValueContainer);
	if(0 == err){
		printf("服务器无响应\n");
		return;
	}
	unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);					//解包TLV信息

	if(0 == strcmp(unpacketTLVContainer[0],"TRUE")){
		puts(unpacketTLVContainer[2]);				//输出TLV中的value
	}else if(0 == strcmp(unpacketTLVContainer[0],"FALSE")){
		printf("错误！\n");
		puts(unpacketTLVContainer[2]);				//输出TLV中的（错误信息）
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
	char sendTLVValueContainer[256];
	char recvTLVValueContainer[256];
	char *unpacketTLVContainer[3];

	int fileLineReal = 0;						//转换成int的文件行数
	int fd;

	fd = createFile(file_name_container_in_Client);
	if(-1 == fd){
		return -1;
	}

	packetTLV(sendTLVValueContainer,"PULL",strlen(file_name_container_in_Server),file_name_container_in_Server);
	sendCmd(socket,sendTLVValueContainer);										//发送提交或拉取命令

	//recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);
	recvMessageWithIOReuse(socket,recvTLVValueContainer);
	unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);					//解包TLV信息

	if(0 == strcmp("TRUE",unpacketTLVContainer[0])){			//拉取文件，成功后TLV的Value字段会有文件的“行数”
		//获取文件行数，并转换为int
		sscanf(unpacketTLVContainer[2],"%d",&fileLineReal);

		printf("打包ACK\n");
		packetTLV(sendTLVValueContainer,"ACK",strlen("recv_Ready"),"recv_Ready");
		sendCmd(socket,sendTLVValueContainer);	
		printf("开始接收文件\n");
		receiveFile(socket,fd,fileLineReal);
		
				
	}else if(0 == strcmp("FALSE",unpacketTLVContainer[0])){
		printf("错误！\n");
		puts(unpacketTLVContainer[2]);				//输出TLV中的value（错误信息）
	}

	closeFile(fd);
	return 0;
}

/**********************************
推送文件命令
描述:客户端向服务器发送一条TLV消息：Type为PUSH，value为文件路径
	 如果服务器上文件存在，则服务器返回的TLV消息中，Type为FALSE，value为读取的文件的“行数”
	 如果服务器上文件不存在，则服务器返回的TLV消息中，Type为True
**********************************/
int pushFileCmd(int socket,char* file_name_container_in_Client,char *file_name_container_in_Server){
	char sendTLVValueContainer[256];
	char recvTLVValueContainer[256];
	char *unpacketTLVContainer[3];		//TLV拆包容器
	char *file_container[1024];			//读取文件容器
	char userConfirmContainer;

	int fd;
	int fileLine_raw = 0;						
	char fileLine[10];									//转换成char的文件行数
	int userConfirmSize;

	fd = openFile(file_name_container_in_Client);
	if(-1 == fd){
		return -1;
	}

	packetTLV(sendTLVValueContainer,"PUSH",strlen(file_name_container_in_Server),file_name_container_in_Server);
	sendCmd(socket,sendTLVValueContainer);			//发送PUSH请求和文件的路径给服务器判断

	recvMessageWithIOReuse(socket,recvTLVValueContainer);
	unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);					//解包TLV信息

	fileLineReal = readFile(fd,file_container);
	sprintf(fileLine,"%d",fileLine_raw);			//将整型转化为字符串

	if(0 == strcmp("TRUE",unpacketTLVContainer[0])){	//服务器回应TRUE表明服务器不存在文件冲突
		packetTLV(sendTLVValueContainer,"ACK",strlen(fileLine),fileLine);	//打包给服务端的回应信息

	}else if(0 == strcmp("FALSE",unpacketTLVContainer[0])){
		printf("服务器上已存在该文件，要覆盖吗?（y/s）");
		scanf("%c",&userConfirmContainer);
		if(userConfirmContainer == 'y'){

		}else
	}
}