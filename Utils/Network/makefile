OBJDIR = obj
VPATH = src
ALL:$(OBJDIR) CmdTransUtils.o FileTransUtils.o InitUtils.o TransmissionUtils.o

$(OBJDIR):
	mkdir -p ./$@

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