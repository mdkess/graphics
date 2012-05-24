CC=g++
CXXFLAGS=-Wall --std=c++0x -ggdb
LDLIBS=-lglut -lGLEW -lGL

all: triangle quad

clean:
	rm triangle quad
