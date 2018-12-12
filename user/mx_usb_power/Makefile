# History:
# Date		Author			Comment
# 03-19-2014	Jared Wu.		For turning on/of the USB ports
# 09-16-2014	Jared Wu.		Build .deb package
#

-include $(ROOTDIR)/.config
#CONFIG_PRODUCT value: DA-820, EXPC-1519 or MC-7270-DC-CP
CONFIG_PRODUCT?=DA-820

EXEC = mx_usb_power

.PHONY: clean romfs install uninstall package_install

all: $(EXEC)

$(EXEC): $(EXEC).o
	$(CC) -o $@ $(EXEC).o

romfs: $(EXEC)
	mkdir -p $(ROMFSDIR)/usr/bin
	$(ROMFSINST) /usr/bin/$(EXEC)
	strip -s $(ROMFSDIR)/usr/bin/$(EXEC)

install: package_install
	install $(EXEC) "$(DESTDIR)/usr/bin/"
	strip -s $(DESTDIR)/usr/bin/$(EXEC)

package_install:
	mkdir -v -p "$(DESTDIR)/usr/bin"
	install $(EXEC) "$(DESTDIR)/usr/bin/"

uninstall:
	-/bin/rm -rf $(DESTDIR)/usr/bin/$(EXEC)


clean:
	-/bin/rm -rf $(EXEC) *.o \
	fakeroot \
	*.deb \
	*_src.tar.gz \
	tags

