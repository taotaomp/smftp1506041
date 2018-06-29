/*在最终编译时要加上"-lcrypt"*/
#include <sys/types.h>
#include <shadow.h>
#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/*
struct spwd {
	char *sp_namp;     			//Login name 
    char *sp_pwdp;      		//Encrypted password 
	long  sp_lstchg;   			//Date of last change (measured in days since 1970-01-01 00:00:00 +0000 (UTC)) 
	long  sp_min;       		//Min # of days between changes 
	long  sp_max;       		//Max # of days between changes 
	long  sp_warn;      		//# of days before password expires to warn user to change it 
	long  sp_inact;     		//# of days after password expires until account is disabled 
	long  sp_expire;    		//Date when account expires (measured in days since 1970-01-01 00:00:00 +0000 (UTC)) 
	unsigned long sp_flag;   	//Reserved 
};
*/

/*技术解释
1、Linux将用户信息存储在/etc/shadow文件中，shadow.h头文件提供访问shadow文件的接口
2、Linux密码使用salt加密，shadow文件中存放的密码是一段加密字符串，比对密码的关键，
	在于获取shadow文件中密码（加密字符串)的salt值，
	然后将用户输入的密码使用获取到的salt加密后，与shadow中的密码进行比对
*/
#ifndef userinfo
#define userinfo struct spwd
#endif

void getSalt(char *saltContainer,char *passwdInShadow);
/******************************
通过用户名获取用户信息
******************************/
userinfo* getUserInfo(userinfo* usr_info_container,char* user_name){
	usr_info_container = getspnam(user_name);
	if(NULL == usr_info_container){
		perror("getspnam");
		return NULL;
	}
	return usr_info_container;
}

/*****************************
比对用户名和密码
0 same
-1 not same
*****************************/
int compareUserPassword(userinfo* usr_info_container,char* user_password){
	char saltContainer[128];

	getSalt(saltContainer,usr_info_container->sp_pwdp);
	//crypt(userpasswd,salt)将用户输入的密码用获取到的salt进行加密，若用户输入的密码正确，则加密生成的字符串与shadow中的密码相同
	if(0 == strcmp(usr_info_container->sp_pwdp,crypt(user_password,saltContainer))){		
		return 0;
	}else{
		return -1;
	}
}
/*****************************
获取shadow中密码（加密字符串）的salt
描述：在第三个$前面的字符串是salt
*****************************/
void getSalt(char *saltContainer,char *passwdInShadow){
    int i;
    int saltSplitFlag = 0;
    int saltSplitFlagCount = 0;
    //取出salt,saltSplitFlag记录密码字符下标
    for(i = 0; ;i++)
    {
        if(passwdInShadow[i] == '$'){
        	saltSplitFlagCount++;
        	saltSplitFlag = i;
        }
        if(saltSplitFlagCount == 3){
        	break;
        }
    }
    //将passwdInShadow中的前saltSplitFlag个字符放入saltContainer中
	strncpy(saltContainer,passwdInShadow,saltSplitFlag);
	saltContainer[saltSplitFlag] = '\0';	//将saltContaine中的最后一位置为\0
}