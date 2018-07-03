#include <sys/socket.h>
#include <netinet/in.h>		//struct sockaddr_in
#include "../prih/UserLogin.h"
#include "../prih/CmdCommit.h"
#include "../../Utils/Network/prih/InitUtils.h"
#include "../../Utils/Other/prih/StandardMacro.h"

int main(int args,int **argv){
	int socketClient;
	struct sockaddr_in sockaddrClient;

	socketClient = initSocket();
	initSockaddr(&sockaddrClient,STANDARD_IP,STANDARD_PORT);

	connectClient(socketClient,&sockaddrClient);
	loginGuide(socketClient);
	commitCmd(socketClient,"ls");
	pullFileCmd(socketClient,"DataStructure.txt","DataStructure.txt");
}