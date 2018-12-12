-include $(ROOTDIR)/.config
CONFIG_PRODUCT?=DA-820

#KERNEL_VERSION=`cat $(ROOTDIR)/$(LINUXDIR)/include/config/kernel.release`
KERNEL_VERSION=`uname -r`

#KDIR:=$(ROOTDIR)/$(LINUXDIR)
KDIR=/lib/modules/$(KERNEL_VERSION)/build

TARGET_MODULE=moxa_usb_power
obj-m += $(TARGET_MODULE).o

# Define the -DDEBUG for debugging
#EXTRA_CFLAGS += -DDEBUG

export CONFIG_PRODUCT KERNEL_VERSION KDIR TARGET_MODULE

.PHONY: modules clean romfs install uninstall package_install

modules: $(TARGET_MODULE).ko 

$(TARGET_MODULE).ko:  $(TARGET_MODULE).c
	@echo "Making modules..."
	$(MAKE) -C $(KDIR) M=`pwd` modules

clean:
	$(MAKE) -C $(KDIR) M=`pwd` clean
	-/bin/rm -rf fakeroot \
	*.deb \
	*_src.tar.gz \
	tags

romfs:
	mkdir -v -p "$(ROMFSDIR)/lib/modules/$(KERNEL_VER)/kernel/drivers/misc"
	$(ROMFSINST) -v $(TARGET_MODULE).ko "/lib/modules/$(KERNEL_VER)/kernel/drivers/misc"

install: package_install
	depmod -a
	if [ -f "/etc/redhat-release" ]; then \
		grep -q "release 7" /etc/redhat-release && make -f Makefile.Redhat7 install || make -f Makefile.Redhat6 install; \
	elif [ -f "/etc/debian_version" ]; then \
		make -f Makefile.Debian install; \
	fi

package_install:
	mkdir -v -p "$(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/misc"
	install $(TARGET_MODULE).ko "$(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/misc/"

uninstall:
	-/bin/rm -f $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/misc/$(TARGET_MODULE).ko
	depmod -a
	if [ -f "/etc/redhat-release" ]; then \
		grep -q "release 7" /etc/redhat-release && make -f Makefile.Redhat7 uninstall || make -f Makefile.Redhat6 uninstall; \
	elif [ -f "/etc/debian_version" ]; then \
		make -f Makefile.Debian uninstall; \
	fi

