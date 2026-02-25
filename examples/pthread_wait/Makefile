all: build/cond_wait build/semaphore

run: build/semaphore
	./build/semaphore

build/cond_wait: build/cond_wait.o
	gcc -lpthread -o build/cond_wait build/cond_wait.o

build/cond_wait.o: cond_wait.c
	mkdir -p build
	gcc -Wall -Wextra -g -c cond_wait.c -o build/cond_wait.o

build/semaphore: build/semaphore.o
	gcc -lpthread -o build/semaphore build/semaphore.o

build/semaphore.o: semaphore.c
	gcc -Wall -Wextra -g -c semaphore.c -o build/semaphore.o

clean:
	rm -rf build