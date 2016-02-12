.PHONY: clean

CC= g++
CXXFLAGS+= -g -std=c++11 -DASIO_STANDALONE -I.
LDLIBS= -L/usr/local/lib -L. -lflic -ldsuid -lossp-uuid -lpthread

all: main

main: main.o format.o

clean:
	rm -rf main main.o format.o
