# History:
# Date		Author			Comment
# 01-14-2014	Jared Wu.		For mini PCI-E disable or reset
#

-include $(ROOTDIR)/.config
#CONFIG_PRODUCT value: V2616A
CONFIG_PRODUCT?=V2616A

EXEC = mx_pcie_reset

all: $(EXEC)

$(EXEC): $(EXEC).o
	$(CC) -o $@ $(EXEC).o

romfs: $(EXEC)
	mkdir -p $(ROMFSDIR)/usr/bin
	$(ROMFSINST) /usr/bin/$(EXEC)
	$(STRIP) -s $(ROMFSDIR)/usr/bin/$(EXEC)

install: package_install
	install $(EXEC) "$(DESTDIR)/usr/bin/"
	strip -s $(DESTDIR)/usr/bin/$(EXEC)

package_install:
	mkdir -v -p "$(DESTDIR)/usr/bin"
	install $(EXEC) "$(DESTDIR)/usr/bin/"

clean distclean:
	-/bin/rm -rf $(EXEC) $(EXEC).o \
	fakeroot \
	*.deb \
	*_src.tar.gz \
	tags

