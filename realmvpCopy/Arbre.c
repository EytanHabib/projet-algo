#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "Arbre.h"

Noeud* creer_noeud(const char* type) {
    Noeud* n = malloc(sizeof(Noeud));
    n->type = strdup(type);
    n->texte = NULL;
    n->enfants = NULL;
    n->nb_enfants = 0;
    return n;
}

void ajouter_enfant(Noeud* parent, Noeud* enfant) {
    parent->enfants = realloc(parent->enfants,
                              sizeof(Noeud*) * (parent->nb_enfants + 1));
    parent->enfants[parent->nb_enfants++] = enfant;
}

void liberer_arbre(Noeud* n) {
    if (!n) return;
    for (int i = 0; i < n->nb_enfants; i++)
        liberer_arbre(n->enfants[i]);
    free(n->enfants);
    free(n->type);
    if (n->texte) free(n->texte);
    free(n);
}
