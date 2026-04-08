#include <stdio.h>
#include <stdlib.h>
#include "Parseur.h"

//#define DEBUG 1
#ifdef DEBUG
    #define DEBUG_TRACE() \
        do {\
            fprintf(stderr, "%s\n", __func__); \
        }while(0)
#else
        #define DEBUG_TRACE()\
            do{\
            }while(0)
#endif

void lire_caractere(t_Parseur* ceci){
    if(fscanf(ceci->le_fichier, "%c", &ceci->le_caractere) != 1) {
        if(!feof(ceci->le_fichier)) {
            fprintf(stderr, "Erreur de lecture.\n");
            exit(EXIT_FAILURE);
        }
    }
    if(!feof(ceci->le_fichier)) {
        printf("%c", ceci->le_caractere);
    } else {
        ceci->le_caractere = EOF;
    }
}

int est_en_fin(const t_Parseur* ceci){
    if(feof(ceci->le_fichier)) {
        printf("\nFin du fichier");
        return 1;
    } else {
        return 0;
    }
}

void consommer(t_Parseur* ceci, char attendu) {
    if(ceci->le_caractere == attendu) {
        lire_caractere(ceci);
    } else {
        fprintf(stderr, "Mauvais resultat pour : %c(%d), %c etait attendu \n", ceci->le_caractere, ceci->le_caractere, attendu);
        exit(EXIT_FAILURE);
    }
}

void terminer(t_Parseur* ceci){
    if(fclose(ceci->le_fichier) != 0) {
        fprintf(stderr, "Erreur de fermeture du fichier");
    }
}

void amorcer(t_Parseur* ceci, char* nom_fichier) {
    ceci->le_fichier = fopen(nom_fichier, "r");
    if(ceci->le_fichier == NULL) {
        fprintf(stderr, "Erreur d'ouverture du fichier %s", nom_fichier);
        exit(EXIT_FAILURE);
    }
    lire_caractere(ceci);
}

void texte_enrichi(t_Parseur* ceci){
    document(ceci);
    annexes(ceci);

}

void document(t_Parseur* ceci){
    consommer(ceci, "<document>");
    contenu(ceci);
    consommer(ceci, "</document>");
}



