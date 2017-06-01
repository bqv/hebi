CC=clang
CFLAGS=-g
CXX=clang++
CXXFLAGS=-pthread -std=c++1y -g -Wall -Wextra -Wpedantic -Werror #-O2
INCLUDES=-I /usr/lib/ghc/include -I /usr/include/python3.5m -I /usr/include/guile/2.0
LDFLAGS=-lstdc++ -lpthread
LIBRARIES=-lstdc++ -lpython3.5m -lguile-2.0 -package text -package directory
GHC=ghc
GHCOPTS=-XForeignFunctionInterface -optl-pthread
CYTHON=cython
OBJECTS= \
 main.o \
 logger.o \
 socket.o \
 queue.o \
 thread.o \
 pipe.o \
 irc/connection.o \
 irc/message.o \
 hydra/message.o \
 hydra/session.o \
 hydra/server.o \
 hydra/client.o \
 hydra/node.o \
 plugin/manager.o \
 plugin/plugin.o \
 plugin/haskell.o \
 plugin/haskell/Plugin.o \
 plugin/haskell/Message.o \
 plugin/python.o \
 plugin/python/plugin.o \
 plugin/lisp.o
EXECUTABLE=hebi

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
		$(GHC) -o $@ -no-hs-main -optc-O $(OBJECTS) $(LDFLAGS) $(LIBRARIES)

main.o: main.cpp logger.hpp irc/connection.hpp hydra/session.hpp thread.hpp
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

pipe.o: pipe.cpp pipe.hpp
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

plugin/manager.o: plugin/manager.cpp plugin/manager.hpp plugin/plugin.cpp irc/message.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

plugin/plugin.o: plugin/plugin.cpp plugin/plugin.hpp irc/message.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

plugin/haskell.o: plugin/haskell.cpp plugin/haskell.hpp plugin/plugin.cpp plugin/haskell/Plugin_stub.h irc/message.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

plugin/haskell/Plugin_stub.h: plugin/haskell/Plugin.o
plugin/haskell/Plugin.o: plugin/haskell/Plugin.hs plugin/haskell/Message.o
		cd plugin/haskell/; $(GHC) -c $(GHCOPTS) -O $(shell basename $<)

plugin/haskell/Message.o: plugin/haskell/Message.hs
		cd plugin/haskell/; $(GHC) -optl-static -O $(shell basename $<)

plugin/python.o: plugin/python.cpp plugin/python.hpp plugin/plugin.cpp plugin/python/plugin.h irc/message.hpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

plugin/python/plugin.c plugin/python/plugin.h: plugin/python/plugin.pyx
		$(CYTHON) $<

plugin/python/plugin.o: plugin/python/plugin.c plugin/python/plugin.h
		$(CC) $(INCLUDES) -c $< -o $@ $(CFLAGS)

plugin/lisp.o: plugin/lisp.cpp plugin/lisp.hpp plugin/plugin.cpp
		$(CXX) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

clean:
		$(RM) $(EXECUTABLE) *.o */*.o */*/*.o */*/*.hi */*/*.c */*/*.h

valgrind:
		valgrind --track-origins=yes --leak-check=full --vgdb-error=0 ./hebi irc.freenode.net
