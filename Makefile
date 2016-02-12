.PHONY: clean

CC= g++
CXXFLAGS+= -std=c++11 -DASIO_STANDALONE -I.
LDLIBS= -lflic -ldsuid -lpthread

all: main

main: main.o format.o

clean:
	rm -rf main main.o
