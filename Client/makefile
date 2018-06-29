OBJDIR = obj
VPATH = src
ALL:Client.o UserLogin.o CmdCommit.o

Client.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

UserLogin.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

CmdCommit.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

clear:
	$(RM) $(OBJDIR)/*.o