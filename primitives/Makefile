CC = gcc
# Add debugging information -g
CFLAGS = -std=c99 -O2 -Wall -finline-functions -Iinclude -g -DDEBUG
AR = ar
AR_FLAGS = -cq
LIB_FILE = libsat.a

SRC = src/satapi.c src/ParseDIMACS.c src/LiteralWatch.c src/VSIDS.c src/main.c  

OBJS=$(SRC:.c=.o)

HEADERS = include/satapi.h include/ParseDIMACS.h include/LiteralWatch.h include/VSIDS.h

#sat: $(OBJS)
#	$(AR) $(AR_FLAGS) $(LIB_FILE) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIB_FILE) sat


EXEC_FILE = sat 
sat: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC_FILE)
