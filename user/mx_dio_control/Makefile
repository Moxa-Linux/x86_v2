
all:
	make -C mx_dio_control/

install: all
	mkdir -p $(DESTDIR)/sbin
	install mx_dio_control/mx_dio_control $(DESTDIR)/sbin/mx_dio_control

distclean: clean

clean:
	$(MAKE) -C mx_dio_control/ clean

