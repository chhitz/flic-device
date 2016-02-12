.PHONY: clean

CC= g++
CXXFLAGS+= -std=c++11 -DASIO_STANDALONE -I. -i/Users/chitz/digitalSTROM/libdsuid/src
LDLIBS= -lflic -ldsuid

all: main

main: main.o

clean:
	rm -rf main main.o
