OBJDIR = obj
VPATH = src
ALL:UserCtrlUtils.o

UserCtrlUtils.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

clear:
	$(RM) $(OBJDIR)/*.o