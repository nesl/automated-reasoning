CC = gcc
CFLAGS = -std=c99 -O0 -Wall -finline-functions -Iinclude -g -DDEBUG
AR = ar
AR_FLAGS = -cq
LIB_FILE = libsat.a

SRC = src/sat_api.c src/ParseDIMACS.c src/LiteralWatch.c src/VSIDS.c src/ConflictAlgorithms.c src/main.c  

OBJS=$(SRC:.c=.o)

HEADERS = include/satapi.h include/ParseDIMACS.h include/LiteralWatch.h include/VSIDS.h include/ConflictAlgorithms.h
#For now don't build as a static lib
#sat: $(OBJS)
#	$(AR) $(AR_FLAGS) $(LIB_FILE) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIB_FILE)

# make it executable just now for debugging purpose
#TODO: remove this in the last submission
EXEC_FILE = sat 
sat: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC_FILE)