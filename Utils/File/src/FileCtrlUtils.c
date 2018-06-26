#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*****************************
打开文件函数
*****************************/
int openFile(char *file_name){
	int err;
	err = open(file_name,O_WRONLY|O_EXCL);
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
return the count of read line
****************************/
int readFile(int fd,char *file_container[]){
	ssize_t read_count;
	int read_line_count = 0;
	int i;

	for(i = 0; ; i++){
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