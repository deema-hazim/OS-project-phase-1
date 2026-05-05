CC = gcc
CFLAGS = -Wall -g -pthread

all: myshell server client demo

myshell: main.o parser.o executor.o errors.o redirects.o pipes.o
	$(CC) $(CFLAGS) -o myshell main.o parser.o executor.o errors.o redirects.o pipes.o

server: server.o scheduler.o parser.o executor.o errors.o redirects.o pipes.o
	$(CC) $(CFLAGS) -o server server.o scheduler.o parser.o executor.o errors.o redirects.o pipes.o

client: client.o errors.o
	$(CC) $(CFLAGS) -o client client.o errors.o

demo: demo.o
	$(CC) $(CFLAGS) -o demo demo.o

demo.o: demo.c
	$(CC) $(CFLAGS) -c demo.c

main.o: main.c myshell.h
	$(CC) $(CFLAGS) -c main.c

server.o: server.c myshell.h scheduler.h
	$(CC) $(CFLAGS) -c server.c

scheduler.o: scheduler.c myshell.h scheduler.h
	$(CC) $(CFLAGS) -c scheduler.c

client.o: client.c myshell.h
	$(CC) $(CFLAGS) -c client.c

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
	rm -rf *.o myshell server client demo demo.dSYM
