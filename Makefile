CC = cc
CFLAGS = -Wall -O2
LDFLAGS =

all: dblp
%.o: %.c
	$(CC) -c $(CFLAGS) $<
dblp: dblp.o json.o
	$(CC) -o $@ $^ $(LDFLAGS)
clean:
	rm -f *.o dblp
