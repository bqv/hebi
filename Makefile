CXX=clang++
CXXFLAGS=-std=c++1y -ggdb -Wall -Wextra -Wpedantic -Werror #-O2
GDB=gdb
LDFLAGS=-lpthread
LIBRARIES=
SOURCES= \
 main.cpp \
 logger.cpp \
 socket.cpp \
 queue.cpp \
 irc/connection.cpp \
 irc/message.cpp \
 hydra/message.cpp \
 hydra/session.cpp \
 hydra/server.cpp \
 hydra/client.cpp \
 hydra/node.cpp
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
