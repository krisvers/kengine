CC=clang
CXX=clang++
AR=ar

CFLAGS=-Ikengine/include -Ilib/include
CLIBSRC=$(shell find ./lib/src -type f -name "*.c")
CLIBOBJ=$(subst .c,.o,$(CLIBSRC))

CXXFLAGS=-Ikengine/include -Ilib/include -std=c++17
CXXLIBS=-Llib/mac/universal -lglfw3

CXXENGINESRC=$(filter-out %_mac.cpp,$(filter-out %_linux.cpp,$(filter-out %_win32.cpp,$(shell find ./kengine/src -type f -name "*.cpp"))))
CXXENGINESRC_WIN32=$(shell find ./kengine/src -type f -name "*_win32.cpp")
CXXENGINESRC_LINUX=$(shell find ./kengine/src -type f -name "*_linux.cpp")
CXXENGINESRC_MAC=$(shell find ./kengine/src -type f -name "*_mac.cpp")

CXXENGINEOBJ=$(subst .cpp,.o,$(CXXENGINESRC))
CXXENGINEOBJ_WIN32=$(subst .cpp,.o,$(CXXENGINESRC_WIN32))
CXXENGINEOBJ_LINUX=$(subst .cpp,.o,$(CXXENGINESRC_LINUX))
CXXENGINEOBJ_MAC=$(subst .cpp,.o,$(CXXENGINESRC_MAC))

OBJFILES=$(shell find ./obj -type f -name "*.o")

.PHONY: obj link-executable link-dylib link-so link-static clean

obj: $(CXXENGINEOBJ) $(CLIBOBJ)

obj-mac: $(CXXENGINEOBJ_MAC)

obj-linux: $(CXXENGINEOBJ_LINUX)

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

variables:
	@echo "CXXENGINESRC: " $(CXXENGINESRC)
	@echo "CXXENGINEOBJ: " $(CXXENGINEOBJ)
	@echo "CXXENGINESRC_WIN32: " $(CXXENGINESRC_WIN32)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o obj/$(subst /,_,$@)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o obj/$(subst /,_,$@)
