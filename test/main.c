#include <stdio.h>
#include "Parseur.h"
#include "Arbre.h"
#include "Rendu.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s fichier.nml\n", argv[0]);
        return 1;
    }

    t_Parseur parseur;
    amorcer(&parseur, argv[1]);

    t_noeud* racine = texte_enrichi(&parseur);

    terminer(&parseur);

    afficher_nanoml(racine);

    liberer_arbre(racine);
    return 0;
}
