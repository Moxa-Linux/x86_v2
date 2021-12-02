
all: modules

modules:
	@echo "Building dkcontrol power management drivers..."
	$(MAKE) -C drivers
	$(MAKE) -C example

distclean: clean

clean:
	$(MAKE) -C drivers clean
	$(MAKE) -C example clean
