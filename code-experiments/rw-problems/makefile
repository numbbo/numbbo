##
## Makefile for the socket_server
##

CCFLAGS = -g -ggdb -std=c89 -pedantic -Wall -Wextra -Wstrict-prototypes -Wshadow -Wno-sign-compare -Wconversion
ifeq ($(OS),Windows_NT)
	LDFLAGS += -lwsock32
endif

all: socket_server

socket_server: socket_server.c
	$(CC) ${CCFLAGS} socket_server.c -o socket_server ${LDFLAGS} 

clean:
	rm -f socket_server
