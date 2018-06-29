OBJDIR = obj
VPATH = src
ALL:CmdTransUtils.o FileTransUtils.o InitUtils.o TransmissionUtils.o

CmdTransUtils.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

FileTransUtils.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

InitUtils.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

TransmissionUtils.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

clear:
	$(RM) $(OBJDIR)/*.o