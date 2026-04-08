#ifndef ARBRE_H
#define ARBRE_H

typedef struct s_noeud {
    char* type;           
    char* valeur;         
    struct s_noeud** enfants;
    int nb_enfants;
} t_noeud;

t_noeud* creer_noeud(const char* type, const char* valeur);
void ajouter_enfant(t_noeud* parent, t_noeud* enfant);
void liberer_arbre(t_noeud* racine);

#endif
