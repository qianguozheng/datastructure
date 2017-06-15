CFLAGS=-levent -lpthread -lssl -lcrypto -levent_openssl
CC=gcc
all: echo-server.c
	$(CC)  -o echo-server echo-server.c $(CFLAGS)
timer:
	$(CC)  -o timer timer.c $(CFLAGS)
signal:
	$(CC)  -o signal signal.c $(CFLAGS)
udp-server:
	$(CC)  -o udpserver udp-server.c $(CFLAGS)
clean:
	rm echo-server
