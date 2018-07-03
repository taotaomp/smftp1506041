/***************************
本文件实现IO复用相关函数
***************************/
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/**************************
套接字文件描述符可读性监视函数
sfd:套接字文件描述符
返回值:0超时|-1错误|>0可读
**************************/
int selectSfdRead(int sfd){
	fd_set readSfd;			//读文件文件描述符集合
	struct timeval timeOut;	//超时时间结构体
	int err;

	FD_ZERO(&readSfd);
	FD_SET(sfd,&readSfd);

	timeOut.tv_sec = 2;
	timeOut.tv_usec = 0;

	err = select(1,readSfd,NULL,NULL,&timeOut);
	if(-1 == err){
		perror("select");
	}
	return err;
}