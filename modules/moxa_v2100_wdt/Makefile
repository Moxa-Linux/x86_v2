-include $(ROOTDIR)/.config
#CONFIG_PRODUCT value: DA-682, DA-683, DA-685, DA-820, DA-682A, EXPC-1519, MC-7100-MP, MC-7200-MP, MC-7270-DC-CP, TC-6100, V2101, V2201, V2403, V2406A, V2426A, V2416A, V2616A
CONFIG_PRODUCT?=DA-820

KERNEL_VERSION=`uname -r`
KDIR:=/lib/modules/$(KERNEL_VERSION)/build

TARGET_MODULE = moxa_v2100_wdt
obj-m += $(TARGET_MODULE).o

EXTRA_CFLAGS+=-isystem $(M)/../moxa_v2100_superio
KBUILD_EXTRA_SYMBOLS+=$(M)/../moxa_v2100_superio/Module.symvers

#EXTRA_CFLAGS+=-D__WDT_TEST__  # Define for watchdog driver testing
EXTRA_CFLAGS+=-D__PROC_READ_ENTRY__

export CONFIG_PRODUCT KERNEL_VERSION KDIR TARGET_MODULE

.PHONY: modules clean romfs install uninstall package_install

modules:
	@echo "Making modules $(TARGET_MODULE).ko ..."
	# Jared, 2013-04-24, 
	# First, to build moxa_v2100_superio
	# Then we need to link with Module.symvers of moxa_superio.ko
	# Or define the KBUILD_EXTRA_SYMBOLS
	#cp -a ../moxa_v2100_superio/Module.symvers .
	$(MAKE) -C $(KDIR) M=`pwd` modules

clean:
	$(MAKE) -C $(KDIR) M=`pwd` clean
	-/bin/rm -rf fakeroot \
	*.deb \
	*_src.tar.gz \
	tags

romfs:
	mkdir -v -p "$(ROMFSDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/watchdog"
	$(ROMFSINST) -v $(TARGET_MODULE).ko "/lib/modules/$(KERNEL_VERSION)/kernel/drivers/watchdog"

install: package_install
	depmod -a
	if [ -f "/etc/redhat-release" ]; then \
		grep -q "release 7" /etc/redhat-release && make -f Makefile.Redhat7 install || make -f Makefile.Redhat6 install; \
	elif [ -f "/etc/debian_version" ]; then \
		make -f Makefile.Debian install; \
	fi

package_install:
	mkdir -v -p "$(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/watchdog"
	install $(TARGET_MODULE).ko "$(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/watchdog/"

uninstall:
	-/bin/rm -f $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/watchdog/$(TARGET_MODULE).ko
	depmod -a
	if [ -f "/etc/redhat-release" ]; then \
		grep -q "release 7" /etc/redhat-release && make -f Makefile.Redhat7 uninstall || make -f Makefile.Redhat6 uninstall; \
	elif [ -f "/etc/debian_version" ]; then \
		make -f Makefile.Debian uninstall; \
	fi

