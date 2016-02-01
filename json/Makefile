CC=gcc
CFLAGS= -I
LDFLAGS=
SUBDIR=

sources += $(wildcard ./*.c)
objs = $(sources:%.c=%.o)
target = json

all : $(target)
$(target): $(objs)
	$(CC) -o $(target) $(objs) -lm

.PHONY : clean
clean:
	-$(RM) $(target) $(objs)
