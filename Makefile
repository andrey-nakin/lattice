CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++11

HEADERS =   calc.hpp statistics.hpp 

OBJS =		lattice.o

LIBS =

TARGET =	lattice

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
