CC=gcc

KERNEL_VERSION=`uname -r`
#KERNEL_VERSION=`cat $(ROOTDIR)/$(LINUXDIR)/include/config/kernel.release`

#KDIR:=$(ROOTDIR)/$(LINUXDIR)
KDIR:=/lib/modules/$(KERNEL_VERSION)/build

TARGET_MODULE = epm3438
obj-m += $(TARGET_MODULE).o

# This define just used for logical test. This will disable the hardware.
#EXTRA_CFLAGS=-D__DEBUG__

export CONFIG_PRODUCT KERNEL_VERSION KDIR TARGET_MODULE

.PHONY: modules clean romfs install uninstall package_install

all: $(TARGET_MODULE).ko

$(TARGET_MODULE).ko: $(TARGET_MODULE).c
	@echo "Making modules..."
	$(MAKE) V=1 -C $(KDIR) M=`pwd` modules

clean:
	$(MAKE) -C $(KDIR) M=`pwd` clean
	-/bin/rm -rf fakeroot \
	*.deb \
	*_src.tar.gz \
	tags

install: package_install
	depmod -a
	if [ -f "/etc/redhat-release" ]; then \
		grep -q "release 7" /etc/redhat-release && make -f Makefile.Redhat7 install || make -f Makefile.Redhat6 install; \
	elif [ -f "/etc/debian_version" ]; then \
		make -f Makefile.Debian install; \
	fi

package_install:
	mkdir -v -p "$(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/char"
	install $(TARGET_MODULE).ko "$(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/char/"

uninstall:
	-/bin/rm -f $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/char/$(TARGET_MODULE).ko
	depmod -a
	if [ -f "/etc/redhat-release" ]; then \
		grep -q "release 7" /etc/redhat-release && make -f Makefile.Redhat7 uninstall || make -f Makefile.Redhat6 uninstall; \
	elif [ -f "/etc/debian_version" ]; then \
		make -f Makefile.Debian uninstall; \
	fi
