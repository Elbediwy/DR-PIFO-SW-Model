CXX=g++
CXXSTD=-std=c++11
CXXFLAGS=$(CXXSTD) -Wall -Wextra -pedantic -g -fPIC
LDFLAGS=-shared

all: pieo.so

pieo.so:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) pieo.cpp -o $@

clean:
	rm -rf pieo.so	
	rm -rf pieo.o	
	rm -rf pieo.lo	
