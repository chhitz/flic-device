.PHONY: clean

ARCH=arm7l

CC= g++
CXXFLAGS+= -g -std=c++11 -DASIO_STANDALONE -Iexternal/asio/asio/include -Iexternal/fliclib-linux/${ARCH}/fliclib-cpp
LDLIBS= -L/usr/local/lib -Lexternal/cppformat/cppformat/ -Lexternal/fliclib-linux/${ARCH}/fliclib-cpp -lcppformat -lflic -ldsuid -lossp-uuid -lpthread

all: main

main: main.o

clean:
	rm -rf main main.o
