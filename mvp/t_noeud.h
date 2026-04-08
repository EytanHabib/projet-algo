#ifndef NOEUD_H
#define NOEUD_H

typedef struct t_noeud {
    char *type;             // "document", "section", "titre", "texte", "liste", "item", "important", "br"
    char *texte;            // contenu textuel (pour titre, mot_simple, etc.)
    struct t_noeud **fils;  // tableau dynamique d’enfants
    int nb_fils;            // nombre d’enfants
} t_noeud;

/* Création d’un nœud */
t_noeud *creer_noeud(const char *type, const char *texte);

/* Ajout d’un enfant */
void ajouter_fils(t_noeud *parent, t_noeud *enfant);

/* Libération mémoire */
void detruire_arbre(t_noeud *n);

#endif
