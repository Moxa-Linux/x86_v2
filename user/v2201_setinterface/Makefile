CC     = $(PREFIX)gcc
EXEC   = setinterface
OBJS   = setinterface.o
STRIP  = $(PREFIX)strip
CFLAGS=
LDFLAGS=

all: $(EXEC)

$(EXEC):	$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)
	$(STRIP) -s $@

install:	$(EXEC)
	install $(EXEC) $(DESTDIR)/bin

romfs:
	$(ROMFSINST) -v $(EXEC) /bin/

clean:
	-/bin/rm -f $(EXEC) *.o

