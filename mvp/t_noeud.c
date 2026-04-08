#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "t_noeud.h"

t_noeud *creer_noeud(const char *type, const char *texte) {
    t_noeud *n = malloc(sizeof(t_noeud));
    n->type = strdup(type);
    n->texte = texte ? strdup(texte) : NULL;
    n->fils = NULL;
    n->nb_fils = 0;
    return n;
}

void ajouter_fils(t_noeud *parent, t_noeud *enfant) {
    parent->fils = realloc(parent->fils, sizeof(t_noeud*) * (parent->nb_fils + 1));
    parent->fils[parent->nb_fils] = enfant;
    parent->nb_fils++;
}

void detruire_arbre(t_noeud *n) {
    if (!n) return;

    for (int i = 0; i < n->nb_fils; i++)
        detruire_arbre(n->fils[i]);

    free(n->fils);
    free(n->type);
    if (n->texte) free(n->texte);
    free(n);
}
