#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>		//MSG_WAITALL
#include "../../Utils/Network/prih/TransmissionUtils.h"
#include "../../Utils/Network/prih/CmdTransUtils.h"

/*************************************
提交CMD命令
socket:套接字文件描述符
cmd_container:命令（ls|pwd|cd）
*************************************/
void commitCmd(int socket,char* cmd_container){
	char sendTLVValueContainer[256];
	char recvTLVValueContainer[256];
	char *unpacketTLVContainer[3];

	packetTLV(sendTLVValueContainer,"CMD",strlen(cmd_container),cmd_container);
	sendCmd(socket,sendTLVValueContainer);	

	recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);						//阻塞等待服务器返回的消息
	unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);					//解包TLV信息

	if(0 == strcmp(unpacketTLVContainer[0],"TRUE")){
		puts(unpacketTLVContainer[2]);				//输出TLV中的value
	}else if(0 == strcmp(unpacketTLVContainer[0],"TRUE")){
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
file_name_container:文件的路径
************************************/
int pullFileCmd(int socket,char* file_name_container){
	char sendTLVValueContainer[256];
	char recvTLVValueContainer[256];
	char *unpacketTLVContainer[3];

	int fileLineReal = 0;						//转换成int的
	int i;

	packetTLV(sendTLVValueContainer,"PULL",strlen(file_name_container),file_name_container);
	sendCmd(socket,sendTLVValueContainer);										//发送提交或拉取命令

	recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);
	unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);					//解包TLV信息

	if(unpacketTLVContainer[0] == "TRUE"){			//拉取文件，成功后TLV的Value字段会有文件的“行数”
		sscanf(unpacketTLVContainer[2],"%d",&fileLine);
		
				
	}else if(unpacketTLVContainer[0] == "FALSE"){
		printf("错误！\n");
		puts(unpacketTLVContainer[2]);				//输出TLV中的value（错误信息）
	}
}

/**********************************
推送文件命令
描述：

**********************************/