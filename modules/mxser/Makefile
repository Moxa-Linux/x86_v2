-include $(ROOTDIR)/.config
#CONFIG_PRODUCT value: DA-682A, DA-820, V2616A
CONFIG_PRODUCT?=DA-820

#KERNEL_VERSION=`cat $(ROOTDIR)/$(LINUXDIR)/include/config/kernel.release`
KERNEL_VERSION=`uname -r`

#KDIR:=$(ROOTDIR)/$(LINUXDIR)
KDIR:=/lib/modules/$(KERNEL_VERSION)/build

TARGET_MODULE = mxser
obj-m += $(TARGET_MODULE).o
TARGET_MODULE1 = mxupcie
obj-m += $(TARGET_MODULE1).o

ifeq ("$(CONFIG_PRODUCT)","V2616A")
EXTRA_CFLAGS +='-D$(CONFIG_PRODUCT)'
endif

.PHONY: modules clean romfs install uninstall package_install

modules: $(TARGET_MODULE).ko $(TARGET_MODULE1).ko

$(TARGET_MODULE).ko: $(TARGET_MODULE).c 
	@echo "Making modules $(TARGET_MODULE).ko ..."
	$(MAKE) -C $(KDIR) M=`pwd` modules

$(TARGET_MODULE1).ko: $(TARGET_MODULE1).c 
	@echo "Making modules $(TARGET_MODULE1).ko ..."
	$(MAKE) -C $(KDIR) M=`pwd` modules

clean:
	$(MAKE) -C $(KDIR) M=`pwd` clean
	-/bin/rm -rf fakeroot \
	*.deb \
	*_src.tar.gz \
	tags

package_install:
	mkdir -p $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/tty/
	install $(TARGET_MODULE).ko $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/tty/
	install $(TARGET_MODULE1).ko $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/tty/

romfs:
	mkdir -v -p "$(ROMFSDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/tty"
	$(ROMFSINST) -v $(TARGET_MODULE).ko "/lib/modules/$(KERNEL_VERSION)/kernel/drivers/tty"

install:
	mkdir -v -p "$(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/tty"
	install $(TARGET_MODULE).ko $(TARGET_MODULE1).ko "$(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/tty"
	depmod -a

uninstall:
	-/bin/rm -f $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/tty/$(TARGET_MODULE).ko \
	$(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/tty/$(TARGET_MODULE1).ko
	depmod -a

