OBJDIR = obj
VPATH = src
UtilsFileOBJ = Utils/File/obj
UtilsNetworkOBJ = Utils/Network/obj
UtilsUserOBJ = Utils/User/obj

ALL:Server.o InterfaceServer.o

Server.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

InterfaceServer.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

clear:
	$(RM) $(OBJDIR)/*.o