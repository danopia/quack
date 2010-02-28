DEFINES =
LIBS = OGRE OIS
CXX = g++
CXXFLAGS = $(shell pkg-config --cflags $(LIBS)) $(DEFINES)
LD = g++
LDFLAGS = $(shell pkg-config --libs $(LIBS))

all:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o Duckgame main.cpp

clean:
	rm -f SampleApp
