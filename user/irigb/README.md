# Compile and install the IRIG-B time sync daemon

## Make the IRIG-B time sync daemon

	root@JaredWheezy64:/home/irigb/mxIrigb_example# make
	for i in unitest mxIrigUtil mxSyncTimeSvc; do \
		make -C $i; \
	done
	make[1]: Entering directory `/home/irigb/mxIrigb_example/unitest'
	g++ unitest.cpp -c
	g++ unitest.o -o unitest -lmxirig -L../mxirig -lrt -lm
	make[1]: Leaving directory `/home/irigb/mxIrigb_example/unitest'
	make[1]: Entering directory `/home/irigb/mxIrigb_example/mxIrigUtil'
	g++ -Wno-write-strings mxIrigUtil.cpp -c
	g++ mxIrigUtil.o -o mxIrigUtil -lmxirig -L../mxirig -lrt -lm
	make[1]: Leaving directory `/home/irigb/mxIrigb_example/mxIrigUtil'
	make[1]: Entering directory `/home/irigb/mxIrigb_example/mxSyncTimeSvc'
	g++  ServiceSyncTime.cpp -c
	g++ ServiceSyncTime.o -o ServiceSyncTime -lmxirig -L../mxirig -lrt -lm
	make[1]: Leaving directory `/home/irigb/mxIrigb_example/mxSyncTimeSvc'

## Install the IRIG-B time sync daemon, ServiceSyncTime.

	root@Moxa:/home/irigb# cp -a mxIrigb/mxSyncTimeSvc/ServiceSyncTime /usr/sbin/

## Launch the ServiceSyncTime manually

	root@Moxa:/home/irigb# /usr/sbin/ServiceSyncTime -t 1 -s 2 -i 10

## Run the ServiceSyncTime automatically at booting

	For Debian system:

	root@Moxa:/home/irigb# cp -a fakeroot/etc/init.d/mx_irigb.sh /etc/init.d/
	root@Moxa:/home/irigb# insserv -d mx_irigb.sh

	For Ubuntu:

	root@Moxa:/home/irigb# cp -a fakeroot/etc/init.d/mx_irigb.sh /etc/init.d/
	root@Moxa:/home/irigb# update-rc.d mx_irigb.sh defaults

	For Redhat Enterprise:

	root@Moxa:/home/irigb# chkconfig --levels 2345 mx_irigb.sh on

