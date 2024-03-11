all:
	clang++ -Ikengine/include $(shell find . -type f -name "*.cpp") -o test
