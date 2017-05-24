CXX=clang++
CXXFLAGS=-pthread -std=c++1y -ggdb -Wall -Wextra -Wpedantic -Werror #-O2
GDB=gdb
LDFLAGS=-Wl,--no-as-needed
LIBRARIES=
OBJECTS= \
 main.o \
 logger.o \
 socket.o \
 queue.o \
 thread.o \
 irc/connection.o \
 irc/message.o \
 hydra/message.o \
 hydra/session.o \
 hydra/server.o \
 hydra/client.o \
 hydra/node.o
EXECUTABLE=hebi

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
		$(CXX) $(INCLUDES) -o $@ $(OBJECTS) $(CXXFLAGS) $(LDFLAGS) $(LIBRARIES)

main.o: main.cpp logger.hpp irc/connection.hpp hydra/session.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

logger.hpp: logger.ipp
logger.o: logger.cpp logger.hpp config.hpp thread.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

socket.hpp: socket.ipp
socket.o: socket.cpp socket.hpp config.hpp logger.hpp queue.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

queue.hpp: queue.ipp
queue.o: queue.cpp queue.hpp logger.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

thread.hpp: thread.ipp
thread.o: thread.cpp thread.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

irc/connection.o: irc/connection.cpp irc/connection.hpp config.hpp logger.hpp socket.hpp queue.hpp thread.hpp irc/message.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

irc/message.o: irc/message.cpp irc/message.hpp config.hpp logger.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

hydra/message.o: hydra/message.cpp hydra/message.hpp config.hpp logger.hpp queue.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

hydra/session.o: hydra/session.cpp hydra/session.hpp config.hpp logger.hpp socket.hpp thread.hpp hydra/message.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

hydra/server.o: hydra/server.cpp hydra/server.hpp config.hpp logger.hpp socket.hpp thread.hpp hydra/node.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

hydra/client.o: hydra/client.cpp hydra/client.hpp config.hpp logger.hpp socket.hpp thread.hpp hydra/node.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

hydra/node.o: hydra/node.cpp hydra/node.hpp socket.hpp queue.hpp hydra/message.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

clean:
		$(RM) $(EXECUTABLE) *.o */*.o

dbg: $(EXECUTABLE)
		$(GDB) $(EXECUTABLE)

rmr:
		$(RM) $(EXECUTABLE) *.o */*.o
		$(MAKE) all
		./$(EXECUTABLE) irc.subluminal.net 6667 || true
