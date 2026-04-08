CC = gcc
CFLAGS = -Wall -Wextra

NAME = nanoml

SRC = main.c Parseur.c Arbre.c Rendu.c
OBJ = main.o Parseur.o Arbre.o Rendu.o

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

Parseur.o: Parseur.c
	$(CC) $(CFLAGS) -c Parseur.c

Arbre.o: Arbre.c
	$(CC) $(CFLAGS) -c Arbre.c

Rendu.o: Rendu.c
	$(CC) $(CFLAGS) -c Rendu.c

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all