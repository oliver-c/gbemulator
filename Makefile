include Makefile.common

CC = clang
CFLAGS = -g -Wall -Wfatal-errors -pedantic `sdl-config --cflags`
LIBS = `sdl-config --libs`

all: $(OBJS)    
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(EXECUTABLE_NAME)		

%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(OBJS)
	rm -rf $(EXECUTABLE_NAME)
