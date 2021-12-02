# This Makefile is for compiling i686/x86_64 libraries in x86_64 Linux

CXX=g++
#CXXFLAGS=-isystem../mxIrigUtil/
CXXFLAGS_I686 = -I /usr/include/x86_64-linux-gnu/c++/4.9 -m32 -march=i686
MACHINE=`uname -m`

all: libmxirig-$(MACHINE).a libmxirig-i686.a

libmxirig-$(MACHINE).a: mxirig.cpp
	# For x86_64 machine, we assume the host is x86_64 machine to build the library
	$(CXX) $(CXXFLAGS) mxirig.cpp -c
	$(AR) crv libmxirig-$(MACHINE).a mxirig.o

libmxirig-i686.a: mxirig.cpp
	make clean
	# For i686 machine, we use -mi686 CXXFLAGS to build the library
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_I686) mxirig.cpp -c
	$(AR) crv libmxirig-i686.a mxirig.o

clean:
	#rm -rf *.o

