CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

# Fichiers sources
SRC = main.c Parseur.c
OBJ = $(SRC:.c=.o)

# Nom de l'exécutable
EXEC = nanoml

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

%.o: %.c P_parseur.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o

mrproper: clean
	rm -f $(EXEC)
