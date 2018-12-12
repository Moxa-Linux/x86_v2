-include $(ROOTDIR)/.config
#CONFIG_PRODUCT value: V2201
CONFIG_PRODUCT?=V2201

KERNEL_VERSION=`cat $(ROOTDIR)/$(LINUXDIR)/include/config/kernel.release`
#KERNEL_VERSION=`uname -r`

KDIR:=$(ROOTDIR)/$(LINUXDIR)
#KDIR=/lib/modules/$(KERNEL_VERSION)/build

TARGET1 = qmi_wwan
TARGET2 = usbnet

obj-m +=  $(TARGET1).o
obj-m +=  $(TARGET2).o

.PHONY: modules clean romfs install uninstall package_install

modules: $(TARGET1).ko

$(TARGET1).ko:
	@echo "Making modules $(TARGET1).ko ..."
	$(MAKE) -C $(KDIR) M=`pwd` modules
	rm -rf fakeroot \
	*.deb \
	*_src.tar.gz \
	tags

clean:
	make -C $(KDIR) M=`pwd` clean

romfs:
	mkdir -v -p "$(ROMFSDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/net/usb"
	$(ROMFSINST) -v $(TARGET1).ko "/lib/modules/$(KERNEL_VERSION)/kernel/drivers/net/usb"
	$(ROMFSINST) -v $(TARGET2).ko "/lib/modules/$(KERNEL_VERSION)/kernel/drivers/net/usb"

install: package_install
	mv $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/net/usb/$(TARGET1).ko $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/net/usb/$(TARGET1).ko.bak
	mv $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/net/usb/$(TARGET2).ko $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/net/usb/$(TARGET2).ko.bak
	cp -a *.ko $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/net/usb/

package_install:
	mkdir -v -p "$(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/char"
	install $(TARGET1).ko "$(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/usb/net/"
	install $(TARGET2).ko "$(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/usb/net/"

uninstall:
	mv $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/net/usb/$(TARGET1).ko.bak $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/net/usb/$(TARGET1).ko
	mv $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/net/usb/$(TARGET2).ko.bak $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/net/usb/$(TARGET2).ko
	depmod -a
