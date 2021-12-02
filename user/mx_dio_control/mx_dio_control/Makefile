FNAME=mx_dio_control

release:
	$(CC) -o $(FNAME) $(FNAME).c dio.c -lpthread
	strip $(FNAME)

debug:
	$(CC) -DDEBUG -o $(FNAME)-dbg $(FNAME).cxx dio.cxx -lpthread

clean:
	-/bin/rm -rf $(FNAME) $(FNAME)-dbg *.o
