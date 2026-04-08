#include <stdio.h>
#include "Parseur.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s fichier.nanoml\n", argv[0]);
        return 1;
    }

    t_Parseur p;
    amorcer(&p, argv[1]);

    texte_enrichi(&p);

    terminer(&p);
    printf("Syntaxe OK\n");
    return 0;
}
