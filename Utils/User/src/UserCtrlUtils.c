#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h> //getpwnam() struct passwd

/*
struct passwd {
    char *pw_name;		//用户名
    char *pw_passwd;	//加密口令
    uid_t pw_uid;		//用户标识符（UID）
    gid_t pw_gid;		//组标识符（GID）
    char *pw_gecos;		//全名
    char *pw_dir;		//主目录
    char *pw_shell;		//默认shell
}
*/
#ifndef userinfo
#define userinfo struct passwd
#endif

/******************************
通过用户名获取用户信息
******************************/
int getUserInfo(userinfo* usr_info_container,char* user_name){
	usr_info_container = getpwnam(user_name);
	if(NULL == usr_info_container){
		perror("getpwnam");
		return -1;
	}
	return 0;
}

/*****************************
比对用户名和密码
0 same
-1 not same
*****************************/
int compareUserPassword(userinfo* usr_info_container,char* user_password){
	if(0 == strcmp(usr_info_container->pw_passwd,user_password)){
		return 0;
	}else{
		return -1;
	}
}

/****************************
用户登录函数(弃用)
****************************/
int loginUser(char *user_name,char *user_password){
	userinfo usr_container;
	getUserInfo(&usr_container,user_name);
	return compareUserPassword(&usr_container,user_password);
}