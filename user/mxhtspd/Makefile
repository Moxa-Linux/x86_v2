
#CONFIG_PRODUCT value: V2616A or V2416A
CONFIG_PRODUCT?=V2616A

CC=gcc
STRIP=strip

#DEBUG = -DDEBUG -g 
CFLAGS= -DCONFIG_PRODUCT="$(CONFIG_PRODUCT)" -Wall $(DEBUG)
LDFLAGS= -L. -lmxhtsp -lpthread

PROG = mxhtspd

LIB = libmxhtsp.so
LIB_FILE = mxhtsp_lx.c
LED_PROG = mxhtspd-setled

all: $(LIB) $(PROG)

$(LIB): $(LIB_FILE) 
	$(CC) $(CFLAGS) -fPIC $(LIB_FILE) -c 
	$(CC) $(CFLAGS) -shared $(LIB_FILE:.c=.o) -o $(LIB) -lc

$(PROG): $(PROG).c $(LIB) $(LED_PROG).c
	$(CC) $(CFLAGS) $(LDFLAGS) $(PROG).c -o $(PROG)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LED_PROG).c -o $(LED_PROG)

clean distclean:
	-/bin/rm -rf $(PROG) $(LIB) $(LED_PROG) mxhtsp_lx.o \
		fakeroot \
		*.deb \
		*_src.tar.gz \
		tags

mytest:
	$(CC) $(CFLAGS) $(LDFLAGS) mytest.c -o mytest

install: $(LIB) $(PROG) package_install
	[ -d $(DESTDIR)/usr/lib ] || mkdir -p $(DESTDIR)/usr/lib
	[ -d $(DESTDIR)/usr/bin ] || mkdir -p $(DESTDIR)/usr/bin
	[ -d $(DESTDIR)/etc/mxhtspd ] || mkdir -p $(DESTDIR)/etc/mxhtspd
	install DEBIAN/etc/udev/rules.d/99-mxhtspd.rules $(DESTDIR)/etc/udev/rules.d/
	install DEBIAN/etc/mxhtspd/* $(DESTDIR)/etc/mxhtspd/
	install DEBIAN/usr/bin/* /usr/bin/
	install $(PROG) $(DESTDIR)/usr/bin/
	install $(LIB) $(DESTDIR)/usr/lib/
	install $(LED_PROG) $(DESTDIR)/usr/bin/
	chmod a+x $(DESTDIR)/usr/bin/mxhtspd* $(DESTDIR)/usr/bin/$(PROG)
	if [ -z `pidof systemd` ]; then \
	       	install DEBIAN/etc/init.d/mxhtspd.sh $(DESTDIR)/etc/init.d/; \
	else \
		install DEBIAN/lib/systemd/system/mxhtspd.service $(DESTDIR)/lib/systemd/system/ ; \
		install DEBIAN/sbin/$(PROG).sh $(DESTDIR)/sbin/ ; \
		chmod a+x $(DESTDIR)/sbin/$(PROG).sh ; \
		$(STRIP) -s $(DESTDIR)/usr/bin/$(PROG) $(DESTDIR)/usr/lib/$(LIB) $(DESTDIR)/usr/bin/$(LED_PROG); \
		install /lib/systemd/system/systemd-udevd.service $(DESTDIR)/etc/systemd/system/ ; \
		sed -i 's/^MountFlags=slave/#MountFlags=slave/' $(DESTDIR)/etc/systemd/system/systemd-udevd.service; \
	fi
	udevadm control --reload-rules

package_install:
	[ -d $(DESTDIR)/usr/lib ] || mkdir -p $(DESTDIR)/usr/lib
	[ -d $(DESTDIR)/usr/bin ] || mkdir -p $(DESTDIR)/usr/bin
	[ -d $(DESTDIR)/etc/mxhtspd ] || mkdir -p $(DESTDIR)/etc/mxhtspd
	[ -d $(DESTDIR)/etc/systemd/system ] || mkdir -p $(DESTDIR)/etc/systemd/system
	[ -d $(DESTDIR)/lib/systemd/system ] || mkdir -p $(DESTDIR)/lib/systemd/system
	[ -d $(DESTDIR)/sbin ] || mkdir -p $(DESTDIR)/sbin
	[ -d $(DESTDIR)/etc/udev/rules.d ] || mkdir -p $(DESTDIR)/etc/udev/rules.d
	install DEBIAN/etc/mxhtspd/* $(DESTDIR)/etc/mxhtspd/
	install DEBIAN/usr/bin/* /usr/bin/
	install DEBIAN/etc/udev/rules.d/99-mxhtspd.rules $(DESTDIR)/etc/udev/rules.d/
	install $(PROG) $(DESTDIR)/usr/bin/
	install $(LIB) $(DESTDIR)/usr/lib/
	install $(LED_PROG) $(DESTDIR)/usr/bin/
	chmod a+x $(DESTDIR)/usr/bin/mxhtspd* $(DESTDIR)/usr/bin/$(PROG)
	if [ -z `pidof systemd` ]; then \
	       	install DEBIAN/etc/init.d/mxhtspd.sh $(DESTDIR)/etc/init.d/; \
	else \
		install DEBIAN/lib/systemd/system/mxhtspd.service $(DESTDIR)/lib/systemd/system/ ; \
		install DEBIAN/sbin/$(PROG).sh $(DESTDIR)/sbin/ ; \
		chmod a+x $(DESTDIR)/sbin/$(PROG).sh ; \
		$(STRIP) -s $(DESTDIR)/usr/bin/$(PROG) $(DESTDIR)/usr/lib/$(LIB) $(DESTDIR)/usr/bin/$(LED_PROG); \
		install /lib/systemd/system/systemd-udevd.service $(DESTDIR)/etc/systemd/system/ ; \
	fi

uninstall:
	-/bin/rm -rf $(DESTDIR)/usr/bin/$(PROG) \
	$(DESTDIR)/usr/lib/$(LIB) \
	$(DESTDIR)/etc/mxhtspd \
	$(DESTDIR)/etc/udev/rules.d/99-mxhtspd.rules \
	$(DESTDIR)/usr/bin/$(PROG) \
	$(DESTDIR)/usr/bin/$(LED_PROG) \
	$(DESTDIR)/usr/lib/$(LIB)
	if [ -z `pidof systemd` ]; then \
	       	-/bin/rm -rf $(DESTDIR)/etc/init.d/mxhtspd.sh ; \
	else \
		-/bin/rm -rf $(DESTDIR)/lib/systemd/system/mxhtspd.service \
		/sbin/$(PROG).sh; \
		sed -i 's/^#MountFlags=slave/MountFlags=slave/' /etc/systemd/system/systemd-udevd.service; \
	fi
	udevadm control --reload-rules


romfs: $(LIB) $(PROG)
	mkdir -p $(ROMFSDIR)/usr/lib \
	$(ROMFSDIR)/usr/bin
	$(ROMFSINST) $(PROG) /usr/bin/
	$(ROMFSINST) $(LIB) /usr/lib/
	$(STRIP) -s $(ROMFSDIR)/usr/bin/$(PROG) \
	$(ROMFSDIR)/usr/lib/$(LIB)

