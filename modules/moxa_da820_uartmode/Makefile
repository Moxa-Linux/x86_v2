-include $(ROOTDIR)/.config
#CONFIG_PRODUCT value: DA-820, EXPC-1519 or MC-7270-DC-CP
CONFIG_PRODUCT?=DA-820

#KERNEL_VERSION=`cat $(ROOTDIR)/$(LINUXDIR)/include/config/kernel.release`
KERNEL_VERSION=`uname -r`

#KDIR:=$(ROOTDIR)/$(LINUXDIR)
KDIR=/lib/modules/$(KERNEL_VERSION)/build

TARGET_MODULE = moxa_uart_mode
obj-m += $(TARGET_MODULE).o
$(TARGET_MODULE)-objs := moxa_uart8250_mode.o
EXTRA_CFLAGS+=-isystem $(M)/../moxa_v2100_superio
KBUILD_EXTRA_SYMBOLS+=-L $(M)/../moxa_v2100_superio/Module.symvers

ifeq ("$(CONFIG_PRODUCT)","MC-7270-DC-CP")
EXTRA_CFLAGS+=-DMC7200
endif
ifeq ("$(CONFIG_PRODUCT)","EXPC-1519")
EXTRA_CFLAGS+=-DEXPC1519
endif
ifeq ("$(CONFIG_PRODUCT)","DA-820")
EXTRA_CFLAGS+=-DDA820
endif

export CONFIG_PRODUCT KERNEL_VERSION KDIR TARGET_MODULE

.PHONY: modules clean romfs install uninstall package_install

modules: $(TARGET_MODULE).ko

$(TARGET_MODULE).ko:
	@echo "Making modules $(TARGET_MODULE).ko ..."
	#cp -a ../moxa_v2100_superio/Module.symvers .
	$(MAKE) -C $(KDIR) M=`pwd` modules

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

