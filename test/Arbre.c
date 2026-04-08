#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Arbre.h"

static char* dupliquer(const char* texte) {
    if (!texte) return NULL;
    char* copie = malloc(strlen(texte) + 1);
    if (!copie) { perror("malloc"); exit(EXIT_FAILURE); }
    strcpy(copie, texte);
    return copie;
}

t_noeud* creer_noeud(const char* type, const char* valeur) {
    t_noeud* noeud = malloc(sizeof(t_noeud));
    if (!noeud) { perror("malloc"); exit(EXIT_FAILURE); }

    noeud->type = dupliquer(type);
    noeud->valeur = dupliquer(valeur);
    noeud->enfants = NULL;
    noeud->nb_enfants = 0;

    return noeud;
}

void ajouter_enfant(t_noeud* parent, t_noeud* enfant) {
    if (!enfant) return;

    parent->enfants = realloc(parent->enfants,(parent->nb_enfants + 1) * sizeof(t_noeud*));
    if (!parent->enfants) { perror("realloc"); exit(EXIT_FAILURE); }

    parent->enfants[parent->nb_enfants] = enfant;
    parent->nb_enfants++;
}

void liberer_arbre(t_noeud* racine) {
    if (!racine) return;

    for (int i = 0; i < racine->nb_enfants; i++)
        liberer_arbre(racine->enfants[i]);

    free(racine->enfants);
    free(racine->type);
    free(racine->valeur);
    free(racine);
}
