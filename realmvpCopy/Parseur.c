#define _POSIX_C_SOURCE 200809L
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Parseur.h"
#include "Arbre.h"




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
   OUTILS POUR BALISES
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

/* ============================================================
   LOOKAHEAD
   ============================================================ */

int lookahead_balise(t_Parseur* ceci, const char* nom) {
    if (ceci->le_caractere != '<')
        return 0;

    long pos = ftell(ceci->le_fichier);
    int c0 = ceci->le_caractere;

    int c = fgetc(ceci->le_fichier);

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

Noeud* texte_enrichi(t_Parseur* ceci) {
    passer_espaces(ceci);

    Noeud* doc = document(ceci);
    Noeud* ann = annexes(ceci);

    ajouter_enfant(doc, ann);
    return doc;
}

Noeud* document(t_Parseur* ceci) {
    balise_ouvrante(ceci, "document");

    Noeud* n = creer_noeud("document");
    Noeud* c = contenu(ceci);
    ajouter_enfant(n, c);

    balise_fermante(ceci, "document");
    return n;
}

Noeud* annexes(t_Parseur* ceci) {
    Noeud* n = creer_noeud("annexes");

    passer_espaces(ceci);
    while (lookahead_balise(ceci, "annexe")) {
        ajouter_enfant(n, annexe(ceci));
        passer_espaces(ceci);
    }

    return n;
}


Noeud* contenu(t_Parseur* ceci) {
    Noeud* n = creer_noeud("contenu");

    passer_espaces(ceci);

    while (!est_en_fin(ceci)) {
        passer_espaces(ceci);

        if (lookahead_balise(ceci, "section"))
            ajouter_enfant(n, section(ceci));

        else if (lookahead_balise(ceci, "titre"))
            ajouter_enfant(n, titre(ceci));

        else if (lookahead_balise(ceci, "liste"))
            ajouter_enfant(n, liste(ceci));

        else if (lookahead_balise(ceci, "important") ||
                 lookahead_balise(ceci, "br") ||
                 ceci->le_caractere != '<')
            ajouter_enfant(n, mot_enrichi(ceci));

        else break;

        passer_espaces(ceci);
    }

    return n;
}
Noeud* annexe(t_Parseur* ceci) {
    balise_ouvrante(ceci, "annexe");

    Noeud* n = creer_noeud("annexe");
    ajouter_enfant(n, contenu(ceci));

    balise_fermante(ceci, "annexe");
    return n;
}
Noeud* texte(t_Parseur* ceci) {
    Noeud* n = creer_noeud("texte");

    passer_espaces(ceci);
    while (!est_en_fin(ceci)) {
        passer_espaces(ceci);

        if (lookahead_balise(ceci, "important") ||
            lookahead_balise(ceci, "br") ||
            ceci->le_caractere != '<')
            ajouter_enfant(n, mot_enrichi(ceci));
        else break;
    }

    return n;
}
Noeud* item(t_Parseur* ceci) {
    balise_ouvrante(ceci, "item");

    Noeud* n = creer_noeud("item");

    passer_espaces(ceci);
    while (!est_en_fin(ceci)) {
        passer_espaces(ceci);

        if (lookahead_balise(ceci, "liste"))
            ajouter_enfant(n, liste(ceci));
        else if (lookahead_balise(ceci, "important") ||
                 lookahead_balise(ceci, "br") ||
                 ceci->le_caractere != '<')
            ajouter_enfant(n, mot_enrichi(ceci));
        else break;
    }

    balise_fermante(ceci, "item");
    return n;
}
Noeud* mot_important(t_Parseur* ceci) {
    balise_ouvrante(ceci, "important");

    Noeud* n = creer_noeud("important");

    passer_espaces(ceci);
    while (!est_en_fin(ceci) && ceci->le_caractere != '<') {
        ajouter_enfant(n, mot_simple(ceci));
        passer_espaces(ceci);
    }

    balise_fermante(ceci, "important");
    return n;
}
Noeud* mot_simple(t_Parseur* ceci) {
    passer_espaces(ceci);

    char buffer[256];
    int i = 0;

    while (!est_en_fin(ceci) &&
           ceci->le_caractere != '<' &&
           !isspace(ceci->le_caractere)) {
        buffer[i++] = ceci->le_caractere;
        lire_caractere(ceci);
    }
    buffer[i] = '\0';

    passer_espaces(ceci);

    Noeud* n = creer_noeud("mot");
    n->texte = strdup(buffer);
    return n;
}

Noeud* section(t_Parseur* ceci) {
    balise_ouvrante(ceci, "section");

    Noeud* n = creer_noeud("section");
    ajouter_enfant(n, contenu(ceci));

    balise_fermante(ceci, "section");
    return n;
}


Noeud* titre(t_Parseur* ceci) {
    balise_ouvrante(ceci, "titre");

    Noeud* n = creer_noeud("titre");
    ajouter_enfant(n, texte(ceci));

    balise_fermante(ceci, "titre");
    return n;
}
Noeud* liste(t_Parseur* ceci) {
    balise_ouvrante(ceci, "liste");

    Noeud* n = creer_noeud("liste");

    passer_espaces(ceci);
    while (lookahead_balise(ceci, "item")) {
        ajouter_enfant(n, item(ceci));
        passer_espaces(ceci);
    }

    balise_fermante(ceci, "liste");
    return n;
}
Noeud* mot_enrichi(t_Parseur* ceci) {
    passer_espaces(ceci);

    if (lookahead_balise(ceci, "important"))
        return mot_important(ceci);

    else if (lookahead_balise(ceci, "br")) {
        balise_br(ceci);
        return creer_noeud("br");
    }

    else
        return mot_simple(ceci);
}

