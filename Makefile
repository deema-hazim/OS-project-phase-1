CC = gcc
CFLAGS = -Wall -g

all: myshell server client

myshell: main.o parser.o executor.o errors.o redirects.o pipes.o
	$(CC) $(CFLAGS) -o myshell main.o parser.o executor.o errors.o redirects.o pipes.o

server: server.o parser.o executor.o errors.o redirects.o pipes.o
	$(CC) $(CFLAGS) -o server server.o parser.o executor.o errors.o redirects.o pipes.o

client: client.o errors.o
	$(CC) $(CFLAGS) -o client client.o errors.o

main.o: main.c myshell.h
	$(CC) $(CFLAGS) -c main.c

server.o: server.c myshell.h
	$(CC) $(CFLAGS) -c server.c

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
	rm -f *.o myshell server client