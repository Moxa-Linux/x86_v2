# History:
# Date		Author			Comment
# 05-21-2013	Jared Wu.		Show the firmware version
#
# Compile:
#	make user/kversion_only

EXEC = kversion
BUILDDATE = "/etc/.builddate"

all:
	#echo "Nothing to do"
	echo "#!/bin/sh" > $(EXEC)
	echo "" >> ./$(EXEC)
	echo "[ -z "\$$PRODUCT_NAME" ] && PRODUCT_NAME=$(CONFIG_PRODUCT)" >> $(EXEC)
	echo "" >> $(EXEC)
	echo "VERSION=$(VERSIONPKG)" >> $(EXEC)
	echo "" >> $(EXEC)
	echo "if [ \"\$$1\" = \"-a\" ]; then" >> $(EXEC)
	echo "	str_version=\"\$$PRODUCT_NAME firmware version \$$VERSION Build \`cat $(BUILDDATE)\`\"" >> $(EXEC)
	echo "else" >> ./$(EXEC)
	echo "	str_version=\"\$$PRODUCT_NAME firmware version \$$VERSION\"" >> $(EXEC)
	echo "fi" >> ./$(EXEC)
	echo "" >> ./$(EXEC)
	echo "echo \$$str_version" >> $(EXEC)
	chmod a+x $(EXEC)

romfs:
	$(ROMFSINST) $(EXEC) /bin/$(EXEC)
	mkdir -p $(ROMFSDIR)/etc
	echo `date +%y%m%d%H` > $(ROMFSDIR)/etc/.builddate
	
clean distclean:
	-rm -rf $(EXEC) $(ROMFSDIR)/etc/.builddate

