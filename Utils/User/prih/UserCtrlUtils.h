#ifndef __USERCTRL__
#define __USERCTRL__

#include <shadow.h>
#define userinfo struct spwd

extern userinfo* getUserInfo(userinfo* usr_info_container,char* user_name);
extern int compareUserPassword(userinfo* usr_info_container,char* user_password);

#endif

