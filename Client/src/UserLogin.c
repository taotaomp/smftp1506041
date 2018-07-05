#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>		//MSG_WAITALL
#include "../../Utils/Network/prih/TransmissionUtils.h"
#include "../../Utils/Network/prih/CmdTransUtils.h"

/****************************************
登录引导函数
socket:套接字文件描述符
返回:登陆是否成功（0/-1）
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
				printf("已登录\n");
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
