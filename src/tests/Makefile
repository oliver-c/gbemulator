# Makefile for tests

CC = ccache gcc
SRC_DIR=..
CFLAGS = -g -Wall -Werror -Wfatal-errors -pedantic `sdl-config --cflags` -I../
LIBS = `sdl-config --libs`
CSRC = main.c GB.c Cartridge.c GUI.c CPU.c CPU_instructions.c MMU.c GPU.c bitOperations.c

OBJS = $(CSRC:.c=.o)

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o test

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(OBJS)
	rm -rf test
