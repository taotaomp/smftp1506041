OBJDIR = obj
VPATH = src
ALL:$(OBJDIR) CmdTransUtils.o FileTransUtils.o InitUtils.o TransmissionUtils.o TransEncryptUtils.o

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

TransEncryptUtils.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^ -lcrypto

clear:
	$(RM) $(OBJDIR)/*.o