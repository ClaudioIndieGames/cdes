all: main

main: main.o
	gcc -lpthread -o main main.o

main.o: main.c event.h array/array.h
	gcc -O0 -Wall -ggdb -c main.c

clean:
	rm -rf main main.exe main.o