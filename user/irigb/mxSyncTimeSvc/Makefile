EXEC=ServiceSyncTime

CXX=g++
ifeq ("$(CONFIG_PRODUCT)","DA-682A")
CXXFLAGS=-D__DA682A__
else
ifeq ("$(CONFIG_PRODUCT)","DA-820")
CXXFLAGS=-D__DA820__
endif
endif
LDFLAGS = -L../mxirig -lmxirig-$(shell uname -m) -lrt -lm
#CXXFLAGS=-D__ENABLE_OUTPUT_FEATURE__	# The output signal feature are not support. You should use the mxIrigUtil or unitest to enable the IRIG-B output feature

all: $(EXEC).o
	$(CXX) $(EXEC).o -o $(EXEC) $(LDFLAGS)

%.o:%.cpp
	$(CXX) $(CXXFLAGS) $? -c

clean:
	rm -rf $(EXEC).o $(EXEC)
