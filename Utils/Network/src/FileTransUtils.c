#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>		//MSG_DONTWAIT MSG_WAITALL
#include "../prih/TransmissionUtils.h"
#include "../../File/prih/FileCtrlUtils.h"
#include "../../Other/prih/IOMoniterUtils.h"	//IO复用功能
#include <string.h>
#include <unistd.h>

/*
char *file_container[1024];
*/

/*************************
发送文件函数
sfd:套接字文件描述符
file_container:文件容器
line_count:文件行数
*************************/
void sendFile(int sfd,char *file_container[],int line_count){
	ssize_t err;
	char TLVValueContainer[1280];					//TLV数据容器1280=1024+256
	int i;

	for(i = 0; i < line_count; i++){
		packetTLV(TLVValueContainer,"FILE",strlen(file_container[i]),file_container[i]);
		err = send(sfd,TLVValueContainer,1280,MSG_DONTWAIT);		//不阻塞，立即返回
		if(-1 == err){
			perror("sendFile");
		}
	}

}

/************************
接收文件函数
sfd:套接字文件描述符
fd:文件描述符
line_count:文件行数
*************************/
void receiveFile(int sfd,int fd,int line_count){
	ssize_t err;
	char TLVValueContainer[1280];							//TLV数据容器1280=1024+256
	char *unpacketTLVContainer[3];							//TLV拆包数据容器
	int i;

	for(i = 0 ; i <= line_count ; i++){
		err = selectSfdRead(sfd);
		if(err > 0){						//大于0表示可读
			err = read(sfd,TLVValueContainer,1280);
			if(-1 == err){
				perror("receiveFile");
			}
			unpacketTLV(TLVValueContainer,unpacketTLVContainer);	//拆包TLV数据
			if(0 == strcmp("FILE",unpacketTLVContainer[0])){
				writeFileForSingleLine(fd,unpacketTLVContainer[2],strlen(unpacketTLVContainer[2]));	//写入一行数据
			}
		}
	}
	return;
}