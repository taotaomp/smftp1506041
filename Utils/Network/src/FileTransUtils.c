#include <netinet/in.h>
#include <stdio.h>
typedef socketaddr_store struct sockaddr_in

/*
char *file_container[1024];
*/

/*************************
文件发送函数
参数：套接字文件描述符，套接字数据结构，文件容器，文件行数
*************************/
void file_send(int sfd,socketaddr_store *sockaddr_to,char *file_container[],int line_count){
	ssize_t err;
	socklen_t len = sizeof(*sockaddr_to);
	int i;

	for(i = 0; i < line_count; i++){
		//err = sendto(sfd,file_container[i],1024,0,(struct sockaddr*)sockaddr_to,len);
		if(-1 == err){
			perror("sendto");
		}
	}

}

/************************
文件接收函数
参数：套接字文件描述符，套接字数据结构，文件容器，文件行数
*************************/
void file_receive(int sfd,socketaddr_store *sockaddr_from,char *file_container[],int line_count){
	ssize_t err;
	socklen_t len = sizeof(*sockaddr_from);
	int i;

	for(i = 0; i < line_count; i++){
		//err = recvfrom(sfd,file_container[i],1024,0,(struct sockaddr*)sockaddr_to,len);
		if(-1 == err){
			perror("recvfrom");
		}
	}
}