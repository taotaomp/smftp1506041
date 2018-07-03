ServerOBJ = Server/obj
ClientOBJ = Client/obj
UtilsFileOBJ = Utils/File/obj
UtilsNetworkOBJ = Utils/Network/obj
UtilsUserOBJ = Utils/User/obj
UtilsOtherOBJ = Utils/Other/obj


ALL:dependency appClient appServer
	
dependency:
	$(MAKE) -C Utils
	$(MAKE) -C Server
	$(MAKE) -C Client

appServer:$(ServerOBJ)/Server.o $(ServerOBJ)/UserLoginProcess.o $(ServerOBJ)/CmdProcess.o $(UtilsFileOBJ)/FileCtrlUtils.o $(UtilsNetworkOBJ)/CmdTransUtils.o $(UtilsNetworkOBJ)/FileTransUtils.o $(UtilsNetworkOBJ)/InitUtils.o $(UtilsNetworkOBJ)/TransmissionUtils.o $(UtilsOtherOBJ)/IOMoniterUtils.o $(UtilsUserOBJ)/UserCtrlUtils.o
	$(CC) -o $@ $^ -g -lcrypt

appClient:$(ClientOBJ)/Client.o $(ClientOBJ)/UserLogin.o $(ClientOBJ)/CmdCommit.o $(UtilsFileOBJ)/FileCtrlUtils.o $(UtilsNetworkOBJ)/CmdTransUtils.o  $(UtilsNetworkOBJ)/FileTransUtils.o $(UtilsNetworkOBJ)/InitUtils.o $(UtilsNetworkOBJ)/TransmissionUtils.o $(UtilsOtherOBJ)/IOMoniterUtils.o
	$(CC) -o $@ $^ -g

clear:
	$(RM) appClient appServer $(ServerOBJ)/*.o $(ClientOBJ)/*.o $(UtilsFileOBJ)/*.o $(UtilsNetworkOBJ)/*.o $(UtilsUserOBJ)/*.o $(UtilsOtherOBJ)/*.o
	