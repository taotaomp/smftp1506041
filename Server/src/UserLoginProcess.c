#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>		//MSG_WAITALL
#include "../../Utils/Network/prih/TransmissionUtils.h"
#include "../../Utils/Network/prih/CmdTransUtils.h"
#include "../../Utils/User/prih/UserCtrlUtils.h"


/*********************************
处理登录函数
*********************************/
int processLogin(int socket){
	char sendTLVValueContainer[256];					//待发送TLV数据容器
	char recvTLVValueContainer[256];					//接收TLV数据容器
	char *unpacketTLVContainer[3];						//TLV拆包数据容器
	userinfo *userinfoContainer;						//用户信息存储容器（在UserCtrlUtils.h中定义）
	int err;

	recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);
	printf("接收到用户名\n");
	unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);
	printf("%s\nLENGTH:%ld\n",unpacketTLVContainer[2],strlen(unpacketTLVContainer[2]));
	if(0 == strcmp("USERNAME",unpacketTLVContainer[0])){
		printf("TLV类型正确\n");
		userinfoContainer = getUserInfo(userinfoContainer,unpacketTLVContainer[2]);
		if(NULL != userinfoContainer){
			printf("用户名正确\n");
			packetTLV(sendTLVValueContainer,"TRUE",5,"VALUE");
			sendCmd(socket,sendTLVValueContainer);	

			recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);
			unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);
			if(0 == strcmp("PASSWD",unpacketTLVContainer[0])){
				printf("TLV类型正确\n");
				printf("%s\nLENGTH:%ld\n",unpacketTLVContainer[2],strlen(unpacketTLVContainer[2]));
				err = compareUserPassword(userinfoContainer,unpacketTLVContainer[2]);
				if(0 == err){
					printf("密码正确\n");
					packetTLV(sendTLVValueContainer,"TRUE",5,"VALUE");
					sendCmd(socket,sendTLVValueContainer);	

					return 0;
				}else{
					printf("密码错误\n");
					return -1;
				}
			}
		}else{
			printf("用户名错误\n");
			packetTLV(sendTLVValueContainer,"FALSE",5,"VALUE");
			sendCmd(socket,sendTLVValueContainer);
			return -1;
		}
	}
	return -1;
}