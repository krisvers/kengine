CC=clang
CXX=clang++
AR=ar

CFLAGS=-Ikengine/include -Ilib/include
CLIBSRC=$(shell find ./lib/src -type f -name "*.c")
CLIBOBJ=$(subst .c,.o,$(CLIBSRC))

CXXFLAGS=-Ikengine/include -Ilib/include -std=c++17
CXXLIBS=-Llib/mac/universal -lglfw3
CXXENGINESRC=$(shell find ./kengine/src -type f -name "*.cpp")
CXXENGINEOBJ=$(subst .cpp,.o,$(CXXENGINESRC))

OBJFILES=$(shell find ./obj -type f -name "*.o")

.PHONY: obj link-executable link-dylib link-so link-static clean

obj: $(CXXENGINEOBJ) $(CLIBOBJ)

link-executable:
	$(CXX) $(OBJFILES) $(CXXLIBS) -o build/kengine

link-dylib:
	$(CXX) -fPIC -shared $(OBJFILES) $(CXXLIBS) -o build/libkengine.dylib

link-so:
	$(CXX) -fPIC -shared $(OBJFILES) $(CXXLIBS) -o build/libkengine.so

link-static:
	$(AR) rc build/libkengine.a $(OBJFILES)

clean:
	rm -rf build obj
	mkdir build obj

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o obj/$(subst /,_,$@)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o obj/$(subst /,_,$@)
