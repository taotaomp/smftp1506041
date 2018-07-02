#include<stdio.h>
#include<sys/types.h>
#include<strings.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>

/***********************************
socket文件描述符生成函数
***********************************/
int initSocket(){
	int err;
	err = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == err){
		perror("initSocket");
		exit(EXIT_FAILURE);
	}
	return err;
}

/***********************************
套接字数据结构赋值函数
sockaddr_temp:套接字数据结构指针
ipaddr:IP地址
portnum:端口号
***********************************/
void initSockaddr(struct sockaddr_in *sockaddr_temp,char* ipaddr,int portnum){
	sockaddr_temp->sin_family  = AF_INET;
	sockaddr_temp->sin_port = htons(portnum);
	sockaddr_temp->sin_addr.s_addr = inet_addr(ipaddr);
	bzero(&(sockaddr_temp->sin_zero),8);
}

/***********************************
文件描述符与套接字数据结构绑定函数
socket:套接字文件描述符
sockaddr_temp:套接字数据结构指针
返回值：成功与否（0/-1）
***********************************/
int bindSocket(int socket,struct sockaddr_in *sockaddr_temp){
	int err; 
	err = bind(socket,(struct sockaddr *)sockaddr_temp,sizeof(struct sockaddr));
	if(-1 == err){
		perror("bind");
	}
	return err;
}

/***********************************
侦听函数（适用于服务器端）
socket:套接字文件描述符
max_row:最大侦听数
返回值：成功与否（0/-1）
***********************************/
int listenServer(int socket,int max_row){
	int err; 
	err = listen(socket,max_row);
	if(-1 == err){
		perror("listen");
	}
	return err;
}

/***********************************
接受连接函数（适用于服务器端）
socket:套接字文件描述符
sockaddr_client:套接字数据结构指针
sock_length:套接字长度容器
返回值:套接字文件描述符
***********************************/
int acceptServer(int socket,struct sockaddr_in *sockaddr_client,int *sock_length){
	int err; 
	err = accept(socket,(struct sockaddr *)sockaddr_client,sock_length);
	if(-1 == err){
		perror("accept");
	}
	return err;
}

/***********************************
连接函数
socket:套接字文件描述符
sockaddr_temp:套接字数据结构指针
返回值:成功与否（0/-1）
***********************************/
int connectClient(int socket,struct sockaddr_in *sockaddr_temp){
	int err;
	err = connect(socket,(struct sockaddr *)sockaddr_temp,sizeof(struct sockaddr));
	if(-1 == err){
		perror("connect");
	}
	return err;
}
