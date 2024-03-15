CXX=clang++
CXXFLAGS=-I../kengine/include -L../build -L../lib/mac/universal -std=c++17

.PHONY: build kengine-static

build: kengine-static
	$(CXX) $(CXXFLAGS) main.cpp -lkengine -lglfw3 -framework OpenGL -framework IOKit -framework Cocoa -o kengine-test

clean:
	cd .. && make clean

kengine-static:
	cd .. && make obj && make link-static
