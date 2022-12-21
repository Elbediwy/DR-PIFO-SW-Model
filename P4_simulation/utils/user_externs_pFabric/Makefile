CXX=g++
CXXSTD=-std=c++11
CXXFLAGS=$(CXXSTD) -Wall -Wextra -pedantic -g -fPIC
LDFLAGS=-shared

all: pFabric.so

pFabric.so:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) pFabric.cpp -o $@

clean:
	rm -rf pFabric.so	
	rm -rf pFabric.o	
	rm -rf pFabric.lo	
