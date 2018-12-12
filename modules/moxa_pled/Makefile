-include $(ROOTDIR)/.config
#CONFIG_PRODUCT value: DA-683, DA-710, DA-820, DA-682A, V2201
CONFIG_PRODUCT?=DA-820

#KERNEL_VERSION=`cat $(ROOTDIR)/$(LINUXDIR)/include/config/kernel.release`
KERNEL_VERSION=`uname -r`

#KDIR:=$(ROOTDIR)/$(LINUXDIR)
KDIR:=/lib/modules/$(KERNEL_VERSION)/build

TARGET_MODULE = moxa-pled
obj-m += $(TARGET_MODULE).o

# This define just used for logical test. This will disable the hardware.
ifeq ("$(CONFIG_PRODUCT)","DA-682A")
EXTRA_CFLAGS+= -DDA682A
else
ifeq ("$(CONFIG_PRODUCT)","DA-681A")
EXTRA_CFLAGS+= -DDA681A
else
ifeq ("$(CONFIG_PRODUCT)","DA-682B")
EXTRA_CFLAGS+= -DDA682B
else
ifeq ("$(CONFIG_PRODUCT)","DA-710")
# wade: -DDA710 is necessary
EXTRA_CFLAGS+= -DDA710
else
ifeq ("$(CONFIG_PRODUCT)","V2201")
EXTRA_CFLAGS+= -DV2201
else
ifeq ("$(CONFIG_PRODUCT)","DA-683")
EXTRA_CFLAGS+= -DDA683
else
ifeq ("$(CONFIG_PRODUCT)","DA-820")
EXTRA_CFLAGS+= -DDA820
endif
endif
endif
endif
endif
endif
endif

# Jared, define __WDT_TEST__ for debugging purpose
# EXTRA_CFLAGS+= -D__WDT_TEST__

export CONFIG_PRODUCT KERNEL_VERSION KDIR TARGET_MODULE

.PHONY: modules clean romfs install uninstall package_install

modules: $(TARGET_MODULE).ko

$(TARGET_MODULE).ko: $(TARGET_MODULE).c 
	@echo "Making modules $(TARGET_MODULE).ko ..."
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
