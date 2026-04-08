CC = gcc
CFLAGS = -Wall -Wextra -std=c11

OBJ = main.o parser.o render.o t_noeud.o

all: nanomlMVP.exe

nanomlMVP.exe: $(OBJ)
	$(CC) $(CFLAGS) -o nanomlMVP.exe $(OBJ)

main.o: main.c parser.h render.h t_noeud.h
parser.o: parser.c parser.h t_noeud.h
render.o: render.c render.h t_noeud.h
t_noeud.o: t_noeud.c t_noeud.h

clean:
	rm -f *.o nanomlMVP.exe
