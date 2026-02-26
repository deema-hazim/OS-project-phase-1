CC = gcc
CFLAGS = -Wall -g

all: myshell

myshell: main.o parser.o executor.o errors.o redirects.o pipes.o
	$(CC) $(CFLAGS) -o myshell main.o parser.o executor.o errors.o redirects.o pipes.o

main.o: main.c myshell.h
	$(CC) $(CFLAGS) -c main.c

parser.o: parser.c myshell.h
	$(CC) $(CFLAGS) -c parser.c

executor.o: executor.c myshell.h
	$(CC) $(CFLAGS) -c executor.c

errors.o: errors.c myshell.h
	$(CC) $(CFLAGS) -c errors.c

redirects.o: redirects.c myshell.h
	$(CC) $(CFLAGS) -c redirects.c

pipes.o: pipes.c myshell.h
	$(CC) $(CFLAGS) -c pipes.c

clean:
	rm -f *.o myshell