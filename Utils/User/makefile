OBJDIR = obj
VPATH = src
ALL:$(OBJDIR) UserCtrlUtils.o

$(OBJDIR):
	mkdir -p ./$@

UserCtrlUtils.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

clear:
	$(RM) $(OBJDIR)/*.o