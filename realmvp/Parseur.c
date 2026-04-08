#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Parseur.h"

/* ============================================================
   FONCTIONS DE BASE
   ============================================================ */

void lire_caractere(t_Parseur* ceci){
    int c = fgetc(ceci->le_fichier);
    ceci->le_caractere = (c == EOF ? EOF : c);
}

int est_en_fin(const t_Parseur* ceci){
    return ceci->le_caractere == EOF;
}

void consommer(t_Parseur* ceci, char attendu) {
    if (ceci->le_caractere != attendu) {
        fprintf(stderr, "Erreur: '%c' attendu, trouvé '%c'\n",
                attendu, ceci->le_caractere);
        exit(EXIT_FAILURE);
    }
    lire_caractere(ceci);
}

void terminer(t_Parseur* ceci){
    if (ceci->le_fichier)
        fclose(ceci->le_fichier);
}

void amorcer(t_Parseur* ceci, char* nom_fichier) {
    ceci->le_fichier = fopen(nom_fichier, "r");
    if (!ceci->le_fichier) {
        fprintf(stderr, "Impossible d'ouvrir %s\n", nom_fichier);
        exit(EXIT_FAILURE);
    }
    lire_caractere(ceci);
}

void passer_espaces(t_Parseur* ceci){
    while (ceci->le_caractere != EOF && isspace(ceci->le_caractere)) {
        lire_caractere(ceci);
    }
}

/* ============================================================
   OUTILS POUR BALISES ET MOTS
   ============================================================ */

void consommer_chaine(t_Parseur* ceci, const char* s) {
    while (*s) {
        if (ceci->le_caractere != *s) {
            fprintf(stderr, "Erreur: '%c' attendu dans \"%s\"\n", *s, s);
            exit(EXIT_FAILURE);
        }
        lire_caractere(ceci);
        s++;
    }
}

void balise_ouvrante(t_Parseur* ceci, const char* nom) {
    passer_espaces(ceci);
    consommer(ceci, '<');
    consommer_chaine(ceci, nom);
    passer_espaces(ceci);
    consommer(ceci, '>');
    passer_espaces(ceci);
}

void balise_fermante(t_Parseur* ceci, const char* nom) {
    passer_espaces(ceci);
    consommer(ceci, '<');
    consommer(ceci, '/');
    consommer_chaine(ceci, nom);
    passer_espaces(ceci);
    consommer(ceci, '>');
    passer_espaces(ceci);
}

void balise_br(t_Parseur* ceci) {
    passer_espaces(ceci);
    consommer(ceci, '<');
    consommer_chaine(ceci, "br");

    if (ceci->le_caractere == '/')
        consommer(ceci, '/');

    consommer(ceci, '>');
    passer_espaces(ceci);
}

void mot_simple(t_Parseur* ceci) {
    passer_espaces(ceci);
    if (ceci->le_caractere == '<' || est_en_fin(ceci)) {
        fprintf(stderr, "mot_simple attendu\n");
        exit(EXIT_FAILURE);
    }
    while (!est_en_fin(ceci) &&
           ceci->le_caractere != '<' &&
           !isspace(ceci->le_caractere)) {
        lire_caractere(ceci);
    }
    passer_espaces(ceci);
}

/* ============================================================
   DECLARATIONS DES NON-TERMINAUX
   ============================================================ */

void document(t_Parseur* ceci);
void annexes(t_Parseur* ceci);
void annexe(t_Parseur* ceci);
void contenu(t_Parseur* ceci);
void section(t_Parseur* ceci);
void titre(t_Parseur* ceci);
void liste(t_Parseur* ceci);
void item(t_Parseur* ceci);
void texte(t_Parseur* ceci);
void mot_enrichi(t_Parseur* ceci);
void mot_important(t_Parseur* ceci);

/* ============================================================
   GRAMMAIRE
   ============================================================ */

void texte_enrichi(t_Parseur* ceci) {
    passer_espaces(ceci);
    document(ceci);
    annexes(ceci);
    passer_espaces(ceci);

    if (!est_en_fin(ceci)) {
        fprintf(stderr, "Texte après fin du document\n");
        exit(EXIT_FAILURE);
    }
}

/* ============================================================
   LOOKAHEAD CORRIGÉ (IGNORE LES BALISES FERMANTES)
   ============================================================ */

int lookahead_balise(t_Parseur* ceci, const char* nom) {
    if (ceci->le_caractere != '<')
        return 0;

    long pos = ftell(ceci->le_fichier);
    int c0 = ceci->le_caractere;

    int c = fgetc(ceci->le_fichier);

    /* Si c'est une balise fermante </...> → ce n'est PAS une balise ouvrante */
    if (c == '/') {
        fseek(ceci->le_fichier, pos, SEEK_SET);
        ceci->le_caractere = c0;
        return 0;
    }

    char buffer[64];
    int i = 0;

    while (c != EOF && isalpha(c) && i < 63) {
        buffer[i++] = c;
        c = fgetc(ceci->le_fichier);
    }
    buffer[i] = '\0';

    fseek(ceci->le_fichier, pos, SEEK_SET);
    ceci->le_caractere = c0;

    return strcmp(buffer, nom) == 0;
}

/* ============================================================
   NON-TERMINAUX
   ============================================================ */

void document(t_Parseur* ceci) {
    balise_ouvrante(ceci, "document");
    contenu(ceci);
    balise_fermante(ceci, "document");
}

void annexe(t_Parseur* ceci) {
    balise_ouvrante(ceci, "annexe");
    contenu(ceci);
    balise_fermante(ceci, "annexe");
}

void annexes(t_Parseur* ceci) {
    passer_espaces(ceci);
    while (lookahead_balise(ceci, "annexe")) {
        annexe(ceci);
        passer_espaces(ceci);
    }
}

void contenu(t_Parseur* ceci) {
    passer_espaces(ceci);

    while (!est_en_fin(ceci)) {
        passer_espaces(ceci);

        if (lookahead_balise(ceci, "section")) section(ceci);
        else if (lookahead_balise(ceci, "titre")) titre(ceci);
        else if (lookahead_balise(ceci, "liste")) liste(ceci);
        else if (lookahead_balise(ceci, "important") ||
                 lookahead_balise(ceci, "br")) mot_enrichi(ceci);
        else if (ceci->le_caractere != '<') mot_enrichi(ceci);
        else break;

        passer_espaces(ceci);
    }
}

void section(t_Parseur* ceci) {
    balise_ouvrante(ceci, "section");
    contenu(ceci);
    balise_fermante(ceci, "section");
}

void titre(t_Parseur* ceci) {
    balise_ouvrante(ceci, "titre");
    texte(ceci);
    balise_fermante(ceci, "titre");
}

void liste(t_Parseur* ceci) {
    balise_ouvrante(ceci, "liste");
    passer_espaces(ceci);

    while (lookahead_balise(ceci, "item")) {
        item(ceci);
        passer_espaces(ceci);
    }

    balise_fermante(ceci, "liste");
}

void item(t_Parseur* ceci) {
    balise_ouvrante(ceci, "item");

    passer_espaces(ceci);
    while (!est_en_fin(ceci)) {
        passer_espaces(ceci);

        if (lookahead_balise(ceci, "liste")) liste(ceci);
        else if (lookahead_balise(ceci, "important") ||
                 lookahead_balise(ceci, "br") ||
                 ceci->le_caractere != '<') mot_enrichi(ceci);
        else break;
    }

    balise_fermante(ceci, "item");
}

void texte(t_Parseur* ceci) {
    passer_espaces(ceci);

    while (!est_en_fin(ceci)) {
        passer_espaces(ceci);

        if (lookahead_balise(ceci, "important") ||
            lookahead_balise(ceci, "br")) mot_enrichi(ceci);
        else if (ceci->le_caractere != '<') mot_enrichi(ceci);
        else break;
    }
}

void mot_enrichi(t_Parseur* ceci) {
    passer_espaces(ceci);

    if (lookahead_balise(ceci, "important")) mot_important(ceci);
    else if (lookahead_balise(ceci, "br")) balise_br(ceci);
    else mot_simple(ceci);
}

void mot_important(t_Parseur* ceci) {
    balise_ouvrante(ceci, "important");

    passer_espaces(ceci);
    while (!est_en_fin(ceci) && ceci->le_caractere != '<') {
        mot_simple(ceci);
        passer_espaces(ceci);
    }

    balise_fermante(ceci, "important");
}
