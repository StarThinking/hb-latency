CC = gcc

CFLAGS = -Wall -ggdb -D__WITH_MURMUR

INCLUDE = -I../include

all: receiver sender

receiver: nhb_receiver.o helper.o
	$(CC) $(CFLAGS) -o nhb_receiver $^ -lpthread -lrt ./liblist.a -L. -lhashtable

sender: nhb_sender.o helper.o
	$(CC) $(CFLAGS) -o nhb_sender $^ -lpthread -lrt

%.o: %.c
	$(CC) $< $(CFLAGS) $(INCLUDE) -c -o $@

clean:
	rm *.o nhb_receiver nhb_sender

.PHONY: clean
