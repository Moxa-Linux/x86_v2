PWD	:= $(shell pwd)
#KDIR	:=/usr/src/linux-headers-3.16.0-0.bpo.4-amd64
KDIR	:=/lib/modules/`uname -r`/build
KMODDIR ?= updates


# enable the drivers
obj-y	:= drivers/net/can/sja1000/
obj-y   := drivers/net/can/
#obj-y	+= net/can/


all: modules

modules: 
	@echo "Building socketcan modules..."
	$(MAKE) V=1 -C $(KDIR) M=$(PWD) modules

install: all
	mkdir -p $(DESTDIR)/lib/modules/`uname -r`/updates/drivers/net/can/sja1000/
	install drivers/net/can/can-dev.ko $(DESTDIR)/lib/modules/`uname -r`/updates/drivers/net/can/
	install drivers/net/can/sja1000/sja1000.ko $(DESTDIR)/lib/modules/`uname -r`/updates/drivers/net/can/sja1000/
	install drivers/net/can/sja1000/plx_pci.ko $(DESTDIR)/lib/modules/`uname -r`/updates/drivers/net/can/sja1000/

distclean: clean

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

