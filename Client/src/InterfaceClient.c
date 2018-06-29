#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>		//MSG_WAITALL
#include "../../Utils/Network/prih/TransmissionUtils.h"
#include "../../Utils/Network/prih/CmdTransUtils.h"

/****************************************
登录引导函数
socket:套接字文件描述符
返回: 0:登录成功。-1:登录失败。
****************************************/
int loginGuide(int socket){
	int i = 0;
	char userNameContainer[256];						//用户名容器
	int userNameLength;									//用户名长度容器
	char passwdContainer[256];							//密码容器
	int passwdLength;									//密码长度容器
	char sendTLVValueContainer[256];					//待发送TLV数据容器
	char recvTLVValueContainer[256];					//接收TLV数据容器
	char *unpacketTLVContainer[3];						//TLV拆包数据容器

	printf("FTP文件传输\n");
	while(i < 3){
		i++;
		//用户名校验
		printf("请输入用户名\n");
		userNameLength = read(1,userNameContainer,1024);
		userNameContainer[userNameLength-1] = '\0';										//使用read(0,xx,xx);进行输入数据时候，回把确认的回车，算成一位，
		userNameLength--;																//附在输入的数据后面，所以应该将最后一位回车去掉（改为'\0'）
																						//同时返回的长度也要减去1

		packetTLV(sendTLVValueContainer,"USERNAME",userNameLength,userNameContainer);	//TLV封装用户名信息
		sendCmd(socket,sendTLVValueContainer);											//发送封装好的用户名信息
		recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);							//阻塞等待服务器返回的消息
		unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);						//解封TLV数据
		if(0 == strcmp("TRUE",unpacketTLVContainer[0])){								//判断TLV的value是否为TRUE，是则表明用户名正确
			//密码校验
			printf("请输入密码\n");
			passwdLength = read(1,passwdContainer,1024);
			passwdContainer[passwdLength-1] = '\0';										//使用read(0,xx,xx);进行输入数据时候，回把确认的回车，算成一位，
			passwdLength--;																//附在输入的数据后面，所以应该将最后一位回车去掉（改为'\0'）
																						//同时返回的长度也要减去1

			printf("正在登录...\n");
			packetTLV(sendTLVValueContainer,"PASSWD",passwdLength,passwdContainer);		//TLV封装密码信息
			sendCmd(socket,sendTLVValueContainer);										//发送封装好的密码信息
			recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);						//阻塞等待服务器返回的消息
			unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);					//解封TLV数据
			if(0 == strcmp("TRUE",unpacketTLVContainer[0])){							//判断TLV的value是否为TRUE，是则表明密码正确
				return 0;
			}else{
				printf("密码错误！\n");
				continue;
			}
		}else{
			printf("此用户不存在！\n");
			continue;
		}
		
	}
	return -1;
}

/*************************************
提交命令
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

	if(unpacketTLVContainer[0] == "TRUE"){
		puts(unpacketTLVContainer[2]);				//输出TLV中的value
	}else if(unpacketTLVContainer[0] == "FALSE"){
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

	char fileLineRecved[4];					//接收的存储文件的行数
	int fileLineRecvedLength;
	int fileLineReal = 0;						//转换成int的
	int i;

	packetTLV(sendTLVValueContainer,"PULL",strlen(file_name_container),file_name_container);
	sendCmd(socket,sendTLVValueContainer);										//发送提交或拉取命令

	recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);
	unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);					//解包TLV信息

	if(unpacketTLVContainer[0] == "TRUE"){			//拉取文件，成功后TLV的Value字段会有文件的“行数”
		fileLineRecvedLength = strlen(unpacketTLVContainer[2]);
		for (i = fileLineRecvedLength; i > 0; i--){
			fileLineReal = fileLineReal + (unpacketTLVContainer[2][i]-48);
		}		
	}else if(unpacketTLVContainer[0] == "FALSE"){
		printf("错误！\n");
		puts(unpacketTLVContainer[2]);				//输出TLV中的value（错误信息）
	}
}

/**********************************
推送文件命令
描述：

**********************************/