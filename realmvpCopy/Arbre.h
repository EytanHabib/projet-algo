#ifndef ARBRE_H
#define ARBRE_H

typedef struct Noeud {
    char* type;
    char* texte;
    struct Noeud** enfants;
    int nb_enfants;
} Noeud;

Noeud* creer_noeud(const char* type);
void ajouter_enfant(Noeud* parent, Noeud* enfant);
void liberer_arbre(Noeud* n);

#endif
