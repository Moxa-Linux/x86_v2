-include $(ROOTDIR)/.config
#CONFIG_PRODUCT value: DA-682A
CONFIG_PRODUCT?=DA-682A

EXEC=setinterface

CFLAGS += -I.

all: $(EXEC).c

$(EXEC).c:	$(EXEC).o
	$(CC) $(EXEC).o -o $(EXEC)

$(EXEC).o:
	$(CC) $(CFLAGS) -c $(EXEC).c

romfs:
	$(ROMFSINST) $(EXEC) /bin/$(EXEC)
	$(STRIP) -s $(ROMFSDIR)/bin/$(EXEC)

install: package_install
	install $(EXEC) "$(DESTDIR)/bin/"
	strip -s $(DESTDIR)/bin/$(EXEC)

package_install:
	mkdir -v -p "$(DESTDIR)/bin"
	install $(EXEC) "$(DESTDIR)/bin/"

uninstall:
	-/bin/rm -rf $(DESTDIR)/bin/$(EXEC)

clean distclean:
	-/bin/rm -fr $(EXEC) $(EXEC).o \
	fakeroot \
	*.deb \
	*_src.tar.gz \
	tags
