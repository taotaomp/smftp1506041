OBJDIR = obj
VPATH = src
ALL:$(OBJDIR) FileCtrlUtils.o

$(OBJDIR):
	mkdir -p ./$@

FileCtrlUtils.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

clear:
	$(RM) $(OBJDIR)/*.o