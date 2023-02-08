CC = g++
CFLAGS += -Wall
FILE = server.cpp
FILE2 = client.cpp

all: server client

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^

server: server.o
	$(CC) $(CFLAGS) -o $@ $^

client: client.o
	$(CC) $(CFLAGS) -o $@ $^


clean:
	rm -f *.o server client
