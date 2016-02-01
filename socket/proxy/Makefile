CC=gcc
CFLAGS= -I.
LDFLAGS=
SUBDIR=
# source code file *.c
#sources = $(wildcard util/*.c) $(wildcard sort/*.c) $(wildcard crypt/*.c)

sources += $(wildcard ./*.c)
objs = $(sources:%.c=%.o)
target = main

all : $(target)
$(target): $(objs)
	$(CC) -g -o $(target) $(objs)

.PHONY : clean
clean:
	-$(RM) $(target) $(objs)
