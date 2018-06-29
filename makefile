ServerOBJ = Server/obj
ClientOBJ = Client/obj
UtilsFileOBJ = Utils/File/obj
UtilsNetworkOBJ = Utils/Network/obj
UtilsUserOBJ = Utils/User/obj


ALL:relay appClient appServer
	
relay:
	$(MAKE) -C Utils
	$(MAKE) -C Server
	$(MAKE) -C Client

appServer:$(ServerOBJ)/Server.o $(ServerOBJ)/InterfaceServer.o $(UtilsFileOBJ)/FileCtrlUtils.o $(UtilsNetworkOBJ)/CmdTransUtils.o $(UtilsNetworkOBJ)/FileTransUtils.o $(UtilsNetworkOBJ)/InitUtils.o $(UtilsNetworkOBJ)/TransmissionUtils.o $(UtilsUserOBJ)/UserCtrlUtils.o
	$(CC) -o $@ $^ -g -lcrypt

appClient:$(ClientOBJ)/Client.o $(ClientOBJ)/InterfaceClient.o $(UtilsFileOBJ)/FileCtrlUtils.o $(UtilsNetworkOBJ)/CmdTransUtils.o  $(UtilsNetworkOBJ)/FileTransUtils.o $(UtilsNetworkOBJ)/InitUtils.o $(UtilsNetworkOBJ)/TransmissionUtils.o
	$(CC) -o $@ $^ -g

clear:
	$(RM) appClient appServer $(ServerOBJ)/*.o $(ClientOBJ)/*.o $(UtilsFileOBJ)/*.o $(UtilsNetworkOBJ)/*.o $(UtilsUserOBJ)/*.o
	