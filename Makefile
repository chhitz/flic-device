.PHONY: clean

CC= g++
CXXFLAGS+= -g -std=c++11 -DASIO_STANDALONE -I.
LDLIBS= -L/usr/local/lib -Lexternal/cppformat/cppformat/ -L. -lcppformat -lflic -ldsuid -lossp-uuid -lpthread

all: main

main: main.o

clean:
	rm -rf main main.o format.o
