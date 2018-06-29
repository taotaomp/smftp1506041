OBJDIR = obj
VPATH = src
UtilsFileOBJ = Utils/File/obj
UtilsNetworkOBJ = Utils/Network/obj
UtilsUserOBJ = Utils/User/obj

ALL:Server.o UserLoginProcess.o CmdProcess.o

Server.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

UserLoginProcess.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^ 

CmdProcess.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^ 

clear:
	$(RM) $(OBJDIR)/*.o