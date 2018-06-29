OBJDIR = obj
VPATH = src
ALL:Client.o InterfaceClient.o

Client.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

InterfaceClient.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

clear:
	$(RM) $(OBJDIR)/*.o