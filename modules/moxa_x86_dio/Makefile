-include $(ROOTDIR)/.config
#CONFIG_PRODUCT value: V2201, V2403, V2400A, V2416A, V2616A, DA-820, DA-682B, MC-7100-MP, MC-7200-MP
CONFIG_PRODUCT?=DA-820

EXTRA_CFLAGS += -DCONFIG_PRODUCT='"$(CONFIG_PRODUCT)"'
# This define just used for logical test. This will disable the hardware.
#EXTRA_CFLAGS += -DDEBUG
KBUILD_EXTRA_SYMBOLS+=$(M)/../moxa_v2100_superio/Module.symvers

#KERNEL_VERSION=`cat $(ROOTDIR)/$(LINUXDIR)/include/config/kernel.release`
KERNEL_VERSION=`uname -r`

#KDIR:=$(ROOTDIR)/$(LINUXDIR)
KDIR:=/lib/modules/$(KERNEL_VERSION)/build

TARGET_MODULE=moxa_device_dio
obj-m += $(TARGET_MODULE).o
#use multiple files
$(TARGET_MODULE)-objs := dio_main.o $(CONFIG_PRODUCT).o

export CONFIG_PRODUCT KERNEL_VERSION KDIR TARGET_MODULE

.PHONY: modules clean romfs install uninstall package_install

modules: $(TARGET_MODULE).ko 

$(TARGET_MODULE).ko: $(CONFIG_PRODUCT).c dio_main.c dio_main.h
	@echo "Making modules..."
	# This module needs to link with Module.symvers of moxa_superio.ko
	#cp -a ../moxa_v2100_superio/Module.symvers .
	$(MAKE) V=1 -C $(KDIR) M=`pwd` modules

clean:
	$(MAKE) -C $(KDIR) M=`pwd` clean
	-/bin/rm -rf fakeroot \
	*.deb \
	*_src.tar.gz \
	tags

romfs:
	mkdir -v -p "$(ROMFSDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/char"
	$(ROMFSINST) -v $(TARGET_MODULE).ko "/lib/modules/$(KERNEL_VERSION)/kernel/drivers/char"

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
