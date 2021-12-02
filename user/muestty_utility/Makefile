
-include $(ROOTDIR)/.config
#CONFIG_PRODUCT value: V2616A
CONFIG_PRODUCT?=V2616A

all: msestty


install: estty_install package_install

package_install:
	mkdir -v -p "$(DESTDIR)/usr/bin"
	install conf/muestty "$(DESTDIR)/usr/bin/"

msdiag:
	cd diag;\
	make;

msmon:
	cd mon;\
	make;

msterm:
	cd term;\
	make;

msestty:
	cd conf;\
	make;


diag_install:
	cd diag;\
	make install

mon_install:
	cd mon;\
	make install

term_install:
	cd term;\
	make install

estty_install:
	cd conf;\
	make install;

clean: global_clean diag_clean mon_clean term_clean estty_clean
	-/bin/rm -rf fakeroot \
	*.deb \
	*_src.tar.gz \
	tags

global_clean:
	-/bin/rm -f *.o

diag_clean:
	cd diag;\
	make clean

mon_clean:
	cd mon;\
	make clean

term_clean:
	cd term;\
	make clean

estty_clean:
	cd conf;\
	make clean

uninstall: estty_uninstall

diag_uninstall:
	cd diag;\
	make uninstall

mon_uninstall:
	cd mon;\
	make uninstall

term_uninstall:
	cd term;\
	make uninstall

estty_uninstall:
	cd conf;\
	make uninstall

romfs:
	$(ROMFSINST) conf/muestty /bin/muestty
	$(STRIP) -s $(ROMFSDIR)/bin/muestty

