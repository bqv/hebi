CXX=clang++
CXXFLAGS=-std=c++11 -O2 -ggdb -Wall
GDB=gdb
LDFLAGS=
LIBRARIES=
SOURCES= \
 main.cpp \
 logger.cpp
OBJECTS=$(SOURCES: .cpp=.o)
EXECUTABLE=hebi

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
		$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBRARIES)

.cpp.o:
		$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
		$(RM) $(EXECUTABLE) *.o

dbg: $(EXECUTABLE)
		$(GDB) $(EXECUTABLE)

rmr:
		$(RM) $(EXECUTABLE) *.o
		$(MAKE) all
		./$(EXECUTABLE) irc.freenode.net 6667 || true
