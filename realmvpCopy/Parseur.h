#ifndef P_PARSEUR_H
#define P_PARSEUR_H

#include <stdio.h>
#include "Arbre.h"

typedef struct {
    FILE* le_fichier;
    int   le_caractere;
} t_Parseur;


void amorcer(t_Parseur* ceci, char* nom_fichier);
void terminer(t_Parseur* ceci);
void lire_caractere(t_Parseur* ceci);
void consommer(t_Parseur* ceci, char attendu);
void passer_espaces(t_Parseur* ceci);
int  est_en_fin(const t_Parseur* ceci);


Noeud* texte_enrichi(t_Parseur* ceci);


Noeud* document(t_Parseur* ceci);
Noeud* annexes(t_Parseur* ceci);
Noeud* contenu(t_Parseur* ceci);
Noeud* annexe(t_Parseur* ceci);
Noeud* section(t_Parseur* ceci);
Noeud* titre(t_Parseur* ceci);
Noeud* liste(t_Parseur* ceci);
Noeud* item(t_Parseur* ceci);
Noeud* texte(t_Parseur* ceci);
Noeud* mot_enrichi(t_Parseur* ceci);
Noeud* mot_simple(t_Parseur* ceci);
Noeud* mot_important(t_Parseur* ceci);

#endif
