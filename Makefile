.PHONY: clean

ARCH=armv7l

CC= g++
CXXFLAGS+= -g -std=c++11 -Iexternal/fliclib-linux/${ARCH}/fliclib-cpp
LDLIBS= -L/usr/local/lib -Lexternal/cppformat/cppformat/ -Lexternal/fliclib-linux/${ARCH}/fliclib-cpp -lboost_system -lcppformat -lflic -ldsuid -lossp-uuid -lpthread

all: main

main: main.o connection.o

clean:
	rm -rf main main.o connection.o
