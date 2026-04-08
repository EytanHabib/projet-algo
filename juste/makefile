CC=gcc
CFLAGS=-Wall -Wextra -std=c11

OBJ=main.o parser.o

nanoml: $(OBJ)
	$(CC) $(OBJ) -o nanoml

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o nanoml