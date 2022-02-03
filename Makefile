
CC=gcc

CFLAGS=-O3 -Wall -g -pg

all : svlistener send-svc

svlistener : svlistener.o
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS) $(LIBS)

send-svc : send-svc.o
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS) $(LIBS)

.PHONY:
clean:
	-rm *.o send-svc svlistener
