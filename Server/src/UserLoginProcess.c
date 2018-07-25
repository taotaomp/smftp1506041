#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>		//MSG_WAITALL
#include "../../Utils/Network/prih/TransmissionUtils.h"
#include "../../Utils/Network/prih/CmdTransUtils.h"
#include "../../Utils/User/prih/UserCtrlUtils.h"


/*********************************
处理登录函数
socket:套接字文件描述符
返回值:登陆是否成功（0/-1）
*********************************/
int processLogin(int socket){
	int i = 0;
	char sendTLVValueContainer[256];					//待发送TLV数据容器
	char recvTLVValueContainer[256];					//接收TLV数据容器
	char *unpacketTLVContainer[3];						//TLV拆包数据容器
	userinfo *userinfoContainer;						//用户信息存储容器（在UserCtrlUtils.h中定义）
	int err;

	while(i < 3){
		//阻塞接收用户名信息，接收后解包
		recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);
		unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);

		//判断TLV数据头是否为USERNAME
		if(0 == strcmp("USERNAME",unpacketTLVContainer[0])){
			printf("接收到用户名数据\n");
			//获取系统用户名，判断用户是否存在
			userinfoContainer = getUserInfo(userinfoContainer,unpacketTLVContainer[2]);
			if(NULL != userinfoContainer){		//用户存在
				//封装TLV回复，告知客户端用户存在
				printf("用户名正确\n");
				packetTLV(sendTLVValueContainer,"TRUE",5,"VALUE");
				sendCmd(socket,sendTLVValueContainer);	

				//阻塞接收密码信息，接收后解包
				recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);
				unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);

				//判断TLV数据头是否为PASSWD
				if(0 == strcmp("PASSWD",unpacketTLVContainer[0])){
					//比对用户的密码是否和系统一致
					err = compareUserPassword(userinfoContainer,unpacketTLVContainer[2]);
					if(0 == err){	//一致
						//封装TLV回复，告知客户端用户登录成功
						printf("密码正确\n");
						packetTLV(sendTLVValueContainer,"TRUE",5,"VALUE");
						sendCmd(socket,sendTLVValueContainer);	

						return 0;
					}else{
						//封装TLV回复，告知客户端用户登录失败
						printf("密码错误\n");
						packetTLV(sendTLVValueContainer,"FALSE",5,"VALUE");
						sendCmd(socket,sendTLVValueContainer);
						continue;
					}
				}
			}else{
				//封装TLV回复，告知客户端用户不存在
				printf("用户名不存在\n");
				packetTLV(sendTLVValueContainer,"FALSE",5,"VALUE");
				sendCmd(socket,sendTLVValueContainer);
				continue;
			}
		}else{
			continue;
		}
	}
	//三次错误则登录失败
	return -1;
}