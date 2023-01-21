# Dennis Audu [148463193]
# daudu@myseneca.ca

CC = g++
LIBS = -L . libPidUtil.a
CFLAGS = -Wall -I.

all: output

output: lab1.cpp
	 $(CC) $(CFLAGS) -o output lab1.cpp $(LIBS)
	chmod +x output
clean:
	rm -f *.o output
