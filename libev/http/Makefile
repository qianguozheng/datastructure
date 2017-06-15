CC=gcc
CFLAGS= -I./include/
LDFLAGS= -lev -lsodium
SUBDIR=sort
# source code file *.c
sources = $(wildcard util/*.c) $(wildcard sort/*.c) $(wildcard crypt/*.c)

sources += $(wildcard ./*.c)
objs = $(sources:%.c=%.o)
target = main

all : $(target)
$(target): $(objs)
	$(CC) -o $(target) $(objs) $(LDFLAGS)

.PHONY : clean
clean:
	-$(RM) $(target) $(objs)
