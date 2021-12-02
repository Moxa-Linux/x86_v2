# History:
# Date		Author			Comment
# 05-21-2013	Jared Wu.		Show the cpu loading
#
# Compile:
#	make user/cpu_only


EXEC = cpu

all: $(EXEC)

$(EXEC): $(EXEC).c
	$(CC) $(EXEC).c -o $(EXEC)

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean distclean:
	-rm -f $(EXEC) *.elf *.o

