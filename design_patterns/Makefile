CC=g++
CFLAGS= 
LDFLAGS=
#SUBDIR=sort
# source code file *.c
#sources = $(wildcard util/*.c) $(wildcard sort/*.c)

sources += $(wildcard ./*.cpp)
objs = $(sources:%.cpp=%.o)
target = main

all : $(target)
$(target): $(objs)
	$(CC) -o $(target) $(objs)

.PHONY : clean
clean:
	-$(RM) $(target) $(objs)
