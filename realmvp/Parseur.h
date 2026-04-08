#ifndef P_PARSEUR_H
#define P_PARSEUR_H

#include <stdio.h>

/* Structure du parseur */
typedef struct {
    FILE* le_fichier;
    int   le_caractere;
} t_Parseur;

/* Fonctions de base */
void amorcer(t_Parseur* ceci, char* nom_fichier);
void terminer(t_Parseur* ceci);
void lire_caractere(t_Parseur* ceci);
void consommer(t_Parseur* ceci, char attendu);
void passer_espaces(t_Parseur* ceci);
int  est_en_fin(const t_Parseur* ceci);

/* Analyseur nanoml */
void texte_enrichi(t_Parseur* ceci);

#endif
