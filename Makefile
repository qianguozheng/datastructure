CC=gcc
FLAGS= -I./
LDFLAGS=

objects=sort.o main.o
all : $(objects)
	$(CC) $(FLAGS) -o main $(objects)
main.o : main.c
	$(CC)	-c main.c
sort.o : sort.c sort.h
	$(CC)	-c $(FLAGS)	sort.c
clean:
	rm main $(objects)
