#ifndef P_PARSEUR_H
#define P_PARSEUR_H

#include <stdio.h>
#include "Arbre.h"

typedef struct s_Parseur {
    FILE* fichier;
    char caractere;
} t_Parseur;

void amorcer(t_Parseur* parseur, const char* nom_fichier);
void terminer(t_Parseur* parseur);
void lire_caractere(t_Parseur* parseur);
int est_en_fin(const t_Parseur* parseur);
void sauter_espaces(t_Parseur* parseur);
void consommer(t_Parseur* parseur, char attendu);
void consommer_chaine(t_Parseur* parseur, const char* attendu);
int commence_par(t_Parseur* parseur, const char* chaine);

t_noeud* texte_enrichi(t_Parseur* parseur);
t_noeud* document(t_Parseur* parseur);
t_noeud* annexes(t_Parseur* parseur);
t_noeud* annexe(t_Parseur* parseur);
t_noeud* contenu(t_Parseur* parseur);
t_noeud* section(t_Parseur* parseur);
t_noeud* titre(t_Parseur* parseur);
t_noeud* liste(t_Parseur* parseur);
t_noeud* item(t_Parseur* parseur);
t_noeud* liste_texte(t_Parseur* parseur);
t_noeud* texte_liste(t_Parseur* parseur);
t_noeud* texte(t_Parseur* parseur);
t_noeud* mot_enrichi(t_Parseur* parseur);
t_noeud* mot_simple(t_Parseur* parseur);
t_noeud* mot_important(t_Parseur* parseur);
t_noeud* balise_br(t_Parseur* parseur);

#endif
