#include <stdio.h>
#include "Parseur.h"
#include "Arbre.h"
#include "Affichage.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s fichier.nanoml\n", argv[0]);
        return 1;
    }

    t_Parseur p;
    amorcer(&p, argv[1]);

    Noeud* racine = texte_enrichi(&p);

    terminer(&p);

    afficher_document(racine);

    liberer_arbre(racine);
    return 0;
}
