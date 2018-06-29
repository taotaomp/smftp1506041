#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

/**************************************
命令发送函数
socket:套接字文件描述符
cmd_container:待发送命令容器（已TLV封装）
返回:已发送的长度
**************************************/
int sendCmd(int socket,char* cmd_container){
	int err;
	err = write(socket,cmd_container,255);
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
	message_container[0] = '\0';		//初始化message_container
	err = recv(socket,message_container,255,flag);
	if(-1 == err){
		perror("recvMessage");
	}
	return err;
}