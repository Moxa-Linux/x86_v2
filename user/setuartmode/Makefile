# History:
# Date		Author			Comment
# 08-15-2014	Jared Wu.		Configure UART8250 mode as RS-232/422/485
# 09-16-2014	Jared Wu.		Build .deb package
#

-include $(ROOTDIR)/.config
#CONFIG_PRODUCT value: DA-820, EXPC-1519 or MC-7270-DC-CP
#CONFIG_PRODUCT?=DA-820

EXEC=setuartmode

CFLAGS += -I.
ifeq ("$(CONFIG_PRODUCT)","MC-7270-DC-CP")
CFLAGS+=-DMC7200
endif
ifeq ("$(CONFIG_PRODUCT)","MC-7200-MP")
CFLAGS+=-DMC7200
endif

.PHONY: clean romfs install uninstall package_install

all: $(EXEC).c


$(EXEC).c:	$(EXEC).o
	$(CC) $(EXEC).o -o $(EXEC)

$(EXEC).o:
	$(CC) $(CFLAGS) -c $(EXEC).c

romfs:
	$(ROMFSINST) $(EXEC) /bin/$(EXEC)
	strip -s $(ROMFSDIR)/bin/$(EXEC)

install: package_install
	install $(EXEC) "$(DESTDIR)/bin/"
	strip -s $(DESTDIR)/bin/$(EXEC)

package_install:
	mkdir -v -p "$(DESTDIR)/bin"
	install $(EXEC) "$(DESTDIR)/bin/"

uninstall:
	-/bin/rm -rf $(DESTDIR)/bin/$(EXEC)

clean distclean:
	-/bin/rm -rf $(EXEC) $(EXEC).o \
	fakeroot \
	*.deb \
	*_src.tar.gz \
	tags

