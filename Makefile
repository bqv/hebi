CXX=g++
CXXFLAGS=-std=c++11 -O2 #-Wall
LDFLAGS=
LIBRARIES=
SOURCES= \
 main.cpp
OBJECTS=$(SOURCES: .cpp=.o)
EXECUTABLE=ethirc

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
		$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBRARIES)

.cpp.o:
		$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
		$(RM) $(EXECUTABLE) *.o

rmr:
		$(RM) $(EXECUTABLE) *.o
		$(MAKE) all
		./$(EXECUTABLE) irc.freenode.net 6667
