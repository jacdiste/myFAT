CC = gcc
CFLAGS = -g

all: main

main: myFAT.o main.o 
	$(CC) $(CFLAGS) -o main myFAT.o main.o

myFAT.o: myFAT.c myFAT.h
	$(CC) $(CFLAGS) -c myFAT.c 

main.o: main.c myFAT.h
	$(CC) $(CFLAGS) -c main.c 

clean:
	rm -f *.o main