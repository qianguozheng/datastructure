#CC=gcc
CFLAGS= -I./  -fPIC 
LDFLAGS= -ljson-c 
# source code file *.c

sources = $(wildcard ./*.c)
objs = $(sources:%.c=%.o)
target = delay_stat

all : $(target)
$(target): $(objs)
	$(CC) -g -o $(target) $(objs) $(LDFLAGS) $(CFLAGS)

.PHONY : clean
clean:
	-$(RM) $(target) $(objs)
