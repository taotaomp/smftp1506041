#include <sys/socket.h>
#include <netinet/in.h>		//struct sockaddr_in
#include "../prih/InterfaceServer.h"
#include "../../Utils/Network/prih/InitUtils.h"
#include "../../Utils/Other/prih/StandardMacro.h"


/*****************************
主函数
*****************************/
int main(int args,char *argv[]){
	int socketServer,socketClient;
	struct sockaddr_in sockaddrServer,sockaddrClient;
	int sock_length;

	socketServer = initSocket();
	initSockaddr(&sockaddrServer,STANDARD_IP,STANDARD_PORT);
	bindSocket(socketServer,&sockaddrServer);

	listenServer(socketServer,STANDARD_LISTEN_LENGTH);

	while(1){
		socketClient = acceptServer(socketServer,&sockaddrServer,&sock_length);
		dealLogin(socketClient);
	}
}
