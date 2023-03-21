CC=g++
CFLAGS=-I
CFLAGS+=-Wall
FILES=Logger.cpp
FILES+=Automobile.cpp
FILES+=TravelSimulator.cpp
LIBS=-lpthread

travel: $(FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

server: Server.cpp
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o travel
	rm -f *.o server
	
all: travel server
