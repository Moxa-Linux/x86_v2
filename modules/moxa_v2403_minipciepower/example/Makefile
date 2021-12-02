
.PHONY: clean install uninstall package_install

all:
	gcc mx_pcie_power_lx.c mx_pcie_power.c -o mx_pcie_power

clean:
	-/bin/rm -rf mx_pcie_power \
	fakeroot \
	*.deb \
	*_src.tar.gz \
	tags

install: package_install
	mkdir -p $(DESTDIR)/sbin/
	install example/mx_pcie_power $(DESTDIR)/sbin/

package_install:
	mkdir -v -p "$(DESTDIR)/sbin"
	install mx_pcie_power "$(DESTDIR)/sbin/"
