CC = gcc
CFLAGS = -O3
CLIB = -lrt -lpthread

SRCS = $(wildcard *.c)
BUILD = $(patsubst %.c, %, $(SRCS))

all: $(BUILD)

.c:
	$(CC) $(CFLAGS) $< -o $@ $(CLIB)

clean:
	rm -rf latency-test *.o

