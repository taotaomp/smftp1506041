#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*****************************
创建文件函数
*****************************/
int createFile(char *file_name){
	int err;
	err = creat(file_name,S_IRWXU);		//创建文件，权限值0700
}

/*****************************
打开文件函数
*****************************/
int openFile(char *file_name){
	int err;
	err = open(file_name,O_RDWR|O_EXCL);
	if(-1 == err){
		perror("file_open");
	}
	return err;
}

/**************************
关闭文件函数
**************************/
int closeFile(int fd){
	int err;
	err = close(fd);
	if(-1 == err){
		perror("close_file");
	}
	return err;
}

/*
char *file_container[1024];
*/

/****************************
文件读取函数
return the count of read line(从0开始计数)
****************************/
int readFile(int fd,char *file_container[]){
	ssize_t read_count;
	int read_line_count = 0;
	int i;

	for(i = 0; ; i++){
		file_container[i] = (char *)malloc(1024);		//这里反复实验了多次，在调用这个指针当读取的文件的容器时，
														//必须先用malloc做动态内存分配，不然会报“bad Address”错误(日了狗了)
		read_count = read(fd,file_container[i],1024);

		if(-1 == read_count){
			perror("read_file");
			exit(EXIT_FAILURE);
		}else if(0 == read_count){	//EOF
			break;
		}

		read_line_count++;
	} 
	
	return read_line_count;
}

/*************************
文件写入函数
*************************/
void writeFile(int fd,char *file_container[],int line_conut){
	int i;

	for(i = 0; i < line_conut; i++){
		write(fd,file_container[i],strlen(file_container[i]));
	}
}