all: build/main

run: build/main
	./build/main

build/main: build/main.o build/cpu.o build/module.o
	gcc -o build/main build/main.o build/cpu.o build/module.o

build/main.o: main.c
	mkdir -p build
	gcc -O0 -Wall -Wextra -g -c main.c -o build/main.o

build/cpu.o: cpu.c
	mkdir -p build
	gcc -O0 -Wall -Wextra -g -c cpu.c -o build/cpu.o

build/module.o: module.c
	mkdir -p build
	gcc -O0 -Wall -Wextra -g -c module.c -o build/module.o

clean:
	rm -rf build