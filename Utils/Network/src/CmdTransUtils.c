#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "../../Other/prih/IOMoniterUtils.h"	//IO复用功能
#include "../prih/TransEncryptUtils.h"			//传输加密功能
#include "../../File/prih/FileCtrlUtils.h"		//用于读取存放的秘钥

/**************************************
命令发送函数
socket:套接字文件描述符
cmd_container:待发送命令容器（已TLV封装）
返回值:已发送的长度
**************************************/
int sendCmd(int socket,char* cmd_container){
	int err;

	char *encryptContainer;		//加密输出结果容器指针
	char key_container[8];		//秘钥存放容器
	char *file_container[1];	//秘钥文件读取存放容器
	int fd;						//秘钥文件描述符
	int i;

	if(0 == strlen(cmd_container) % 8){
		encryptContainer = (char*)malloc(strlen(cmd_container));
	}else{
		encryptContainer = (char*)malloc(((strlen(cmd_container) / 8) + 1) * 8);
	}

	//获取本地存放的密钥
	fd = openFile(".key");
	readFile(fd,file_container);
	for(i = 0 ; i < 8 ; i++){
		key_container[i] = file_container[0][i];
	}
	closeFile(fd);
	//开始加密
	EncryptValue(cmd_container,encryptContainer,key_container,0);

	err = write(socket,encryptContainer,256);
	if(-1 == err){
		perror("sendCmd");
	}
	return err;
}

/*************************************
消息接收函数（基于recv()）
socket:套接字文件描述符
cmd_container:接收的消息的容器（TLV形式封装）
flag:recv()操作方式
返回:已接收的长度
*************************************/
int recvMessage(int socket,char* message_container,int flag){
	int err;

	char *decryptContainer;			//解密输出结果容器指针
	char key_container[8];			//秘钥存放容器
	char *file_container[1];		//秘钥文件读取存放容器
	int fd;							//秘钥文件描述符
	int i;

	decryptContainer = (char*)malloc(256);
	
	//获取本地存放的密钥
	fd = openFile(".key");
	readFile(fd,file_container);
	for(i = 0 ; i < 8 ; i++){
		key_container[i] = file_container[0][i];
	}
	closeFile(fd);

	message_container[0] = '\0';		//初始化message_container
	err = recv(socket,message_container,256,flag);

	//开始解密
	EncryptValue(message_container,decryptContainer,key_container,1);

	message_container[0] = '\0';
	strcpy(message_container,decryptContainer);

	if(-1 == err){
		perror("recvMessage");
	}
	return err;
}

/***********************************
带IO复用的消息接收函数
socket:套接字文件描述符
cmd_container:接收的消息的容器（TLV形式封装）
返回:已接收的长度|0超时|-1错误
***********************************/
int recvMessageWithIOReuse(int socket,char* message_container){
	int err;

	char *decryptContainer;			//解密输出结果容器指针
	char key_container[8];			//秘钥存放容器
	char *file_container[1];		//秘钥文件读取存放容器
	int fd;							//秘钥文件描述符
	int i;

	decryptContainer = (char*)malloc(256);
	
	//获取本地存放的密钥
	fd = openFile(".key");
	readFile(fd,file_container);
	for(i = 0 ; i < 8 ; i++){
		key_container[i] = file_container[0][i];
	}
	closeFile(fd);

	message_container[0] = '\0';		//初始化message_container

	err = selectSfdRead(socket);		//IO检测函数，使用IO复用技术
	if((-1 == err)){
		perror("selectSfdRead");
	}else if(err > 0){
		err = read(socket,message_container,256);
		//开始解密
		EncryptValue(message_container,decryptContainer,key_container,1);
		message_container[0] = '\0';
		strcpy(message_container,decryptContainer);

		if(-1 == err){
			perror("recvMessage");
		}
	}

	return err;	
}