CXX=g++
CXXSTD=-std=c++11
CXXFLAGS=$(CXXSTD) -Wall -Wextra -pedantic -g -fPIC
LDFLAGS=-shared

all: DR_PIFO.so

DR_PIFO.so:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) DR_PIFO.cpp -o $@

clean:
	rm -rf DR_PIFO.so	
	rm -rf DR_PIFO.o	
	rm -rf DR_PIFO.lo