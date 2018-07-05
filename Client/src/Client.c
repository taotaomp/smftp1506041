#include <sys/socket.h>
#include <netinet/in.h>		//struct sockaddr_in
#include <unistd.h>
#include <string.h>
#include "../prih/UserLogin.h"
#include "../prih/CmdCommit.h"
#include "../../Utils/Network/prih/InitUtils.h"
#include "../../Utils/Other/prih/StandardMacro.h"

int main(int args,int **argv){
	int socketClient;
	struct sockaddr_in sockaddrClient;
	int err;
	char UserInputContainer[256];
	char UserInputHeadContainer[6];
	int UserInputSize;
	int count;
	int i;

	socketClient = initSocket();
	initSockaddr(&sockaddrClient,STANDARD_IP,STANDARD_PORT);

	connectClient(socketClient,&sockaddrClient);
	err = loginGuide(socketClient);
	if(0 == err){
		while(1){
			UserInputSize = read(1,UserInputContainer,256);
			UserInputContainer[UserInputSize-1] = '\0';
			UserInputSize--;

			for(i = 0 ; i < UserInputSize ; i++){
				if(UserInputContainer[i] == ' '){
					count = i;
					break;
				}
			}
			strncpy(UserInputHeadContainer,UserInputContainer,count);
			if(0 == strcmp("PULL",UserInputHeadContainer)){

			}else if(0 == strcmp("PUSH",UserInputHeadContainer)){

			}else if(0 == strcmp("REST",UserInputHeadContainer)){

			}else{
				commitCmd(socketClient,UserInputContainer);
			}
		}
	}
	
	//pullFileCmd(socketClient,"hard.txt","hard.txt");
	pushFileCmd(socketClient,"hard.txt","hard.txt");
	while(1);
}