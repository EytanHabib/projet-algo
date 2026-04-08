CC = gcc
CFLAGS = -Wall -Wextra -std=c11

OBJ = main.o Parseur.o Arbre.o Affichage.o

nanoml: $(OBJ)
	$(CC) $(CFLAGS) -o nanoml $(OBJ)

main.o: main.c Parseur.h Arbre.h Affichage.h
Parseur.o: Parseur.c Parseur.h Arbre.h
Arbre.o: Arbre.c Arbre.h
Affichage.o: Affichage.c Affichage.h Arbre.h

clean:
	rm -f *.o nanoml
