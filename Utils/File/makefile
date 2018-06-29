OBJDIR = obj
VPATH = src
ALL:FileCtrlUtils.o

FileCtrlUtils.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

clear:
	$(RM) $(OBJDIR)/*.o