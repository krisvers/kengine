CXX=clang++
CXXFLAGS=-I../kengine/include -L../build -L../lib/mac/universal -std=c++17

UNAME=$(shell uname -s)

.PHONY: build kengine-static

build: kengine-static
	$(CXX) $(CXXFLAGS) main.cpp -lkengine -lglfw3 -framework OpenGL -framework IOKit -framework Cocoa -o kengine-test

clean:
	cd .. && make clean

kengine-static:
	cd .. && make obj
    ifeq ($(UNAME),Linux)
		cd .. && make obj-linux
    endif
    ifeq ($(UNAME),Darwin)
		cd .. && make obj-mac
    endif
	cd .. && make link-static
