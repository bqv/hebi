CXX=clang++
CXXFLAGS=-std=c++1y -ggdb -Wall -Wextra -Wpedantic -Werror #-O2
GDB=gdb
LDFLAGS=-lpthread
LIBRARIES=
SOURCES= \
 main.cpp \
 logger.cpp \
 irc/socket.cpp \
 irc/connection.cpp
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
		./$(EXECUTABLE) irc.subluminal.net 6667 || true
