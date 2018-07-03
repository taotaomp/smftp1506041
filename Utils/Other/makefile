OBJDIR = obj
VPATH = src
ALL:$(OBJDIR) IOMoniterUtils.o

$(OBJDIR):
	mkdir -p ./$@

IOMoniterUtils.o:%.o:%.c
	$(CC) -c -o $(OBJDIR)/$@ $^

clear:
	$(RM) $(OBJDIR)/*.o