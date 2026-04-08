#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "render.h"
#include "t_noeud.h"

int main(int argc, char *argv[]) {


    if (argc != 2) {
        printf("Usage : %s fichier.nml\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("Erreur ouverture fichier");
        return 1;
    }

    t_noeud *racine = parse_texte_enrichi(f);
    fclose(f);
    printf("DEBUG MAIN: racine->type = %s\n", racine->type);
    printf("DEBUG MAIN: racine->nb_fils = %d\n", racine->nb_fils);
    if (racine->nb_fils >= 1 && racine->fils[0])
        printf("DEBUG MAIN: fils[0]->type = %s\n", racine->fils[0]->type);
    else
        printf("DEBUG MAIN: fils[0] = NULL\n");

    if (racine->nb_fils >= 2 && racine->fils[1])
        printf("DEBUG MAIN: fils[1]->type = %s\n", racine->fils[1]->type);
    else
        printf("DEBUG MAIN: fils[1] = NULL\n");


    if (!racine) {
        printf("Document NanoML INVALIDE ❌\n");
        return 1;
    }

    printf("Document NanoML VALIDE ✅\n\n");

    /* Rendu ASCII */
    render(racine);

    /* Libération mémoire */
    detruire_arbre(racine);

    return 0;
}
