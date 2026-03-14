all: build/module

run: build/module
	./build/module

build/module: build/module.o
	gcc -lpthread -o build/module build/module.o

build/module.o: module.c
	mkdir -p build
	gcc -Wall -Wextra -g -c module.c -o build/module.o

clean:
	rm -rf build