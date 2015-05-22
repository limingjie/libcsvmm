# Check OS
uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifneq (,$(findstring MINGW,$(uname_S)))
	X=.exe
endif

ifeq ($(OS),Windows_NT)
	X=.exe
endif

# Check CXX flag.
ifeq ($(CXX),clang++)
	CXX=clang++
else
	CXX=g++
endif

CXXFLAGS = -Wall -g -O2 -std=c++0x

all: demo$(X)

demo.o: demo.cpp
	@echo "==>Compiling demo.o..."
	$(CXX) -c $(CXXFLAGS) -o demo.o demo.cpp

demo$(X): demo.o
	@echo "==>Linking demo$(X)..."
	$(CXX) -o demo$(X) demo.o

clean:
	@echo "==>Cleaning..."
	rm -rf demo.o
	rm -rf demo$(X)
