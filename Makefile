CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++11

HEADERS =   calc.hpp statistics.hpp 

OBJS =		lattice.o

LIBS =

TARGET =	lattice

CONDOR_TARGET =	$(TARGET).condor

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

$(CONDOR_TARGET):	$(OBJS)
	condor_compile $(CXX) -o $(CONDOR_TARGET) $(OBJS) $(LIBS)
	
all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
