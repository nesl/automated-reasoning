CC = gcc
CFLAGS = -std=c99 -O2 -Wall -finline-functions -Iinclude
AR = ar
AR_FLAGS = -cq
LIB_FILE = libsat.a

SRC = src/sat_api.c

OBJS=$(SRC:.c=.o)

HEADERS = sat_api.h

sat: $(OBJS)
	$(AR) $(AR_FLAGS) $(LIB_FILE) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIB_FILE)
