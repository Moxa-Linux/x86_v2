# History:
# Date		Author			Comment
# 05-21-2013	Jared Wu.		For Ethernet bonding
#
# compile: 
#	make user/ifenslave_only

EXEC=ifenslave

all: $(EXEC)

$(EXEC).c:
	cp -a $(ROOTDIR)/$(LINUXDIR)/Documentation/networking/ifenslave.c .

$(EXEC): $(EXEC).c
	$(CC) -o $@ $(EXEC).c

romfs: $(EXEC)
	$(ROMFSINST) $(EXEC) /sbin/$(EXEC)
	$(STRIP) -s $(ROMFSDIR)/sbin/$(EXEC)

clean distclean:
	rm -fr $(EXEC) $(EXEC).c
