CC = gcc
CFLAGS = -Wall -Wextra

SRC = main.c Parseur.c Arbre.c Rendu.c
OBJ = $(SRC:.c=.o)

NAME = nanoml

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all