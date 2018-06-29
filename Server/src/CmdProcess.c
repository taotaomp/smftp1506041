#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>		//MSG_WAITALL
#include <stdlib.h>		//system()
#include "../../Utils/Network/prih/TransmissionUtils.h"
#include "../../Utils/Network/prih/CmdTransUtils.h"
#include "../../Utils/User/prih/UserCtrlUtils.h"
#include "../../Utils/File/prih/FileCtrlUtils.h"

//函数声明
int processCmdLs(char* resultContainer,char* cmdValue);
/*************************
侦听命令函数
*************************/
void listenCmd(int socket){
	char sendTLVValueContainer[256];					//待发送TLV数据容器
	char recvTLVValueContainer[256];					//接收TLV数据容器
	char *unpacketTLVContainer[3];						//TLV拆包数据容器
	char cmdExecResultContainer[200];					//命令执行结果容器
	int err;

	while(1){
		recvMessage(socket,recvTLVValueContainer,MSG_WAITALL);
		unpacketTLV(recvTLVValueContainer,unpacketTLVContainer);

		if(0 == strcmp("CMD",unpacketTLVContainer[0])){			//判断TLV的Type头是否为CMD
			err = processCmdLs(cmdExecResultContainer,unpacketTLVContainer[2]);
			if(0 == err){
				packetTLV(sendTLVValueContainer,"TRUE",strlen(cmdExecResultContainer),cmdExecResultContainer);
			}else{
				packetTLV(sendTLVValueContainer,"FAlSE",strlen("ls执行错误"),"ls执行错误");
			}
			
			/*if(0 == strcmp("pwd",unpacketTLVContainer[2])){

			}else if(){

			}*/
		}else if(0 == strcmp("PULL",unpacketTLVContainer[0])){	//判断TLV的Type头是否为PULL

		}else if(0 == strcmp("PUSH",unpacketTLVContainer[0])){	//判断TLV的Type头是否为PUSH

		}
	}
	
}

/***********************
CMD:ls 命令处理函数
注释：这里调用Shell执行命令不能用system()函数，使用system函数写入文件后，文件会被占用，
	 后续的函数就不能读取文件，会报bad address错误，改为使用execv()函数问题解决
************************/
int processCmdLs(char* resultContainer,char* cmdValue){
	char shellCmdContainer[10];	//shell命令容器
	int fdTempShellResult;		//文件描述符，用于指向一个临时文件
	char *file_container[1024];	//文件容器
	int readLine;				//用于存放读取的文件的行数
	int i;
	int err;
	char *execvCmdContainer[1];	//系统调用命令的容器

	shellCmdContainer[0] = '\0';		//初始化容器

	fdTempShellResult = createFile(".lstemp");		//创建一个临时文件，存储shell命令的返回信息
	closeFile(fdTempShellResult);					//关闭文件

	strcat(shellCmdContainer,cmdValue);				//使用字符串连接函数，将得到的命令后面附加上“ >> .lstemp”
	strcat(shellCmdContainer," >> .lstemp");		//使其结果存在刚刚创建的临时文件中

	execvCmdContainer[0] = shellCmdContainer;		//将shell命令放入系统调用容器中
	err = execv("bin/bash",execvCmdContainer);		//执行shell命令
	if(0 > err){
		perror("execv");

		execvCmdContainer[0] = "rm .lstemp";
		execv("bin/bash",execvCmdContainer);
		return -1;
	}

	printf("开始打开临时文件\n");
	fdTempShellResult = openFile(".lstemp");				//打开临时文件
	readLine = readFile(fdTempShellResult,file_container);	//读取文件内容，并返回读取的行数
	printf("读取临时文件完成%d \n",readLine);

	resultContainer[0] = '\0';			//初始化结果容器
	for (i = 0; i <= readLine; i++)		//将读取到的结果写入到结果容器中
	{
		printf("写入第%d次\n", i);
		strcat(resultContainer,file_container[i]);
	}
	printf("结果容器写入完成\n");
	closeFile(fdTempShellResult);		//关闭文件

	execvCmdContainer[0] = "rm .lstemp";
	execv("bin/bash",execvCmdContainer);
	return 0;
}