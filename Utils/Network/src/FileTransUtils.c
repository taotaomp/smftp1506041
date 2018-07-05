#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>		//MSG_DONTWAIT MSG_WAITALL
#include "../prih/TransmissionUtils.h"
#include "../prih/TransEncryptUtils.h"			//传输加密功能
#include "../../File/prih/FileCtrlUtils.h"
#include "../../Other/prih/IOMoniterUtils.h"	//IO复用功能
#include <string.h>
#include <unistd.h>
#include <malloc.h>

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
	char TLVValueContainer[1280];				//TLV数据容器1280=1024+256
	int i;

	char *encryptContainer;						//加密输出结果容器指针
	char key_container[8];						//秘钥存放容器
	char *file_container_key[1];				//秘钥文件读取存放容器
	int fd_key;									//秘钥文件描述符

	//获取本地存放的密钥
	fd_key = openFile(".key");
	readFile(fd_key,file_container_key);
	for(i = 0 ; i < 8 ; i++){
		key_container[i] = file_container_key[0][i];
	}
	closeFile(fd_key);

	for(i = 0; i < line_count; i++){
		packetTLV(TLVValueContainer,"FILE",strlen(file_container[i]),file_container[i]);		//封装TLV

		if(0 == strlen(TLVValueContainer) % 8){				//初始化加密输出结果容器
			encryptContainer = (char*)malloc(strlen(TLVValueContainer));
		}else{
			encryptContainer = (char*)malloc(((strlen(TLVValueContainer) / 8) + 1) * 8);
		}

		EncryptValue(TLVValueContainer,encryptContainer,key_container,0);		//加密

		err = send(sfd,encryptContainer,1280,MSG_DONTWAIT);		//发送，不阻塞，立即返回
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

	char *decryptContainer;									//解密输出结果容器指针
	char key_container[8];									//秘钥存放容器
	char *file_container_key[1];							//秘钥文件读取存放容器
	int fd_key;												//秘钥文件描述符

	decryptContainer = (char*)malloc(1280);

	//获取本地存放的密钥
	fd_key = openFile(".key");
	readFile(fd_key,file_container_key);
	for(i = 0 ; i < 8 ; i++){
		key_container[i] = file_container_key[0][i];
	}
	closeFile(fd_key);

	for(i = 0 ; i <= line_count ; i++){
		err = selectSfdRead(sfd);			//IO检测函数，使用IO复用技术
		if(err > 0){						//大于0表示可读
			err = read(sfd,TLVValueContainer,1280);
			if(-1 == err){
				perror("receiveFile");
			}
			//开始解密
			EncryptValue(TLVValueContainer,decryptContainer,key_container,1);
			TLVValueContainer[0] = '\0';
			strcpy(TLVValueContainer,decryptContainer);

			unpacketTLV(TLVValueContainer,unpacketTLVContainer);	//拆包TLV数据

			if(0 == strcmp("FILE",unpacketTLVContainer[0])){
				writeFileForSingleLine(fd,unpacketTLVContainer[2],strlen(unpacketTLVContainer[2]));	//写入一行数据
			}
		}
	}
	return;
}