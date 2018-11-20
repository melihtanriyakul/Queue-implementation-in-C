all: hello
hello: main.o
	gcc main.o -o SRS
main.o: main.c
	gcc -ansi -c main.c
clean:
	rm -rf *o SRS