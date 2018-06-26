#ifndef __USERCTRL__
#define __USERCTRL__ 

extern void getUserInfo(userinfo* usr_info_container,char* user_name);
extern int compareUserPassword(userinfo* usr_info_container,char* user_password);
extern int loginUser(char *user_name,char *user_password);

#endif

