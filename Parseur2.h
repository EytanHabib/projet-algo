#ifndef P_parseur.h 
#define P_parseur.h 

#include <stdio.h>
#include <stdlib.h>

typedef struct s_Parseur{
    FILE* le_fichier;
    char le_caractere;
}t_Parseur;

void lire_caractere(t_Parseur* ceci);
int est_en_fin(const t_Parseur* ceci);
void consommer(t_Parseur* ceci, char attendu);
void terminer(t_Parseur* ceci);
void amorcer(t_Parseur* ceci ,char* nom_fichier);
void texte_enrichi(t_Parseur* ceci);
void document(t_Parseur* ceci);


#endif