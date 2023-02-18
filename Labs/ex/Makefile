CC = g++
CFLAGS += -Wall
FILE = server.cpp
FILE2 = client.cpp

all: server client

server: server.cpp
	$(CC) $(CFLAGS) -o $@ $^

client: client.cpp
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o server client
