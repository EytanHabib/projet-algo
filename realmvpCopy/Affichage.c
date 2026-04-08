#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Affichage.h"
#include "Arbre.h"

#define LARGEUR_MAX 50

/* ------------------------------------------------------------
   OUTILS D'ÉCRITURE
   ------------------------------------------------------------ */

void ligne_vide(int largeur) {
    for (int i = 0; i < largeur; i++) putchar(' ');
    putchar('\n');
}

void ecrire_texte(const char* txt, int largeur) {
    int len = strlen(txt);
    int pos = 0;

    while (pos < len) {
        int n = (len - pos < largeur ? len - pos : largeur);
        fwrite(txt + pos, 1, n, stdout);
        putchar('\n');
        pos += n;
    }
}

/* ------------------------------------------------------------
   AFFICHAGE DES MOTS
   ------------------------------------------------------------ */

void afficher_mot(Noeud* n, char* buffer, int* pos, int largeur) {
    if (*pos + (int)strlen(n->texte) + 1 > largeur)
    {
        buffer[*pos] = '\0';
        printf("%s\n", buffer);
        *pos = 0;
    }

    if (*pos != 0) buffer[(*pos)++] = ' ';

    strcpy(buffer + *pos, n->texte);
    *pos += strlen(n->texte);
}

void afficher_br(char* buffer, int* pos) {
    buffer[*pos] = '\0';
    printf("%s\n", buffer);
    *pos = 0;
}

/* ------------------------------------------------------------
   AFFICHAGE DU TEXTE
   ------------------------------------------------------------ */

void afficher_texte(Noeud* n, int largeur) {
    char buffer[256];
    int pos = 0;

    for (int i = 0; i < n->nb_enfants; i++) {
        Noeud* c = n->enfants[i];

        if (strcmp(c->type, "mot") == 0)
            afficher_mot(c, buffer, &pos, largeur);

        else if (strcmp(c->type, "important") == 0) {
            // IMPORTANT = majuscules
            for (int j = 0; j < c->nb_enfants; j++) {
                Noeud* m = c->enfants[j];
                char tmp[128];
                strcpy(tmp, m->texte);
                for (int k = 0; tmp[k]; k++) tmp[k] = toupper(tmp[k]);

                Noeud faux = { "mot", tmp, NULL, 0 };
                afficher_mot(&faux, buffer, &pos, largeur);
            }
        }

        else if (strcmp(c->type, "br") == 0)
            afficher_br(buffer, &pos);
    }

    if (pos > 0) {
        buffer[pos] = '\0';
        printf("%s\n", buffer);
    }
}

/* ------------------------------------------------------------
   AFFICHAGE DES TITRES
   ------------------------------------------------------------ */

void afficher_titre(Noeud* n, int largeur) {
    // saut de ligne avant
    putchar('\n');

    Noeud* txt = n->enfants[0];
    char buffer[256] = {0};
    int pos = 0;

    for (int i = 0; i < txt->nb_enfants; i++) {
        Noeud* m = txt->enfants[i];
        if (strcmp(m->type, "mot") == 0) {
            if (pos != 0) buffer[pos++] = ' ';
            for (int k = 0; m->texte[k]; k++)
                buffer[pos++] = toupper(m->texte[k]);
        }
    }

    buffer[pos] = '\0';
    ecrire_texte(buffer, largeur);

    putchar('\n');
}

/* ------------------------------------------------------------
   AFFICHAGE DES LISTES
   ------------------------------------------------------------ */

void afficher_item(Noeud* n, int largeur, int indent);

void afficher_liste(Noeud* n, int largeur, int indent) {
    for (int i = 0; i < n->nb_enfants; i++)
        afficher_item(n->enfants[i], largeur, indent);
}

void afficher_item(Noeud* n, int largeur, int indent) {
    // préfixe "# "
    for (int i = 0; i < indent; i++) putchar(' ');
    putchar('#');
    putchar(' ');

    // contenu de l’item
    for (int i = 0; i < n->nb_enfants; i++) {
        Noeud* c = n->enfants[i];

        if (strcmp(c->type, "mot") == 0)
            printf("%s ", c->texte);

        else if (strcmp(c->type, "important") == 0) {
            for (int j = 0; j < c->nb_enfants; j++) {
                Noeud* m = c->enfants[j];
                for (int k = 0; m->texte[k]; k++)
                    putchar(toupper(m->texte[k]));
                putchar(' ');
            }
        }

        else if (strcmp(c->type, "br") == 0)
            putchar('\n');

        else if (strcmp(c->type, "liste") == 0) {
            putchar('\n');
            afficher_liste(c, largeur, indent + 2);
        }
    }

    putchar('\n');
}

/* ------------------------------------------------------------
   AFFICHAGE DU CONTENU
   ------------------------------------------------------------ */

void afficher_contenu(Noeud* n, int largeur) {
    for (int i = 0; i < n->nb_enfants; i++) {
        Noeud* c = n->enfants[i];

        if (strcmp(c->type, "titre") == 0)
            afficher_titre(c, largeur);

        else if (strcmp(c->type, "texte") == 0)
            afficher_texte(c, largeur);

        else if (strcmp(c->type, "liste") == 0)
            afficher_liste(c, largeur, 0);

        else if (strcmp(c->type, "section") == 0)
            afficher_contenu(c->enfants[0], largeur - 2);
    }
}

/* ------------------------------------------------------------
   AFFICHAGE GLOBAL
   ------------------------------------------------------------ */

void afficher_document(Noeud* racine) {
    Noeud* doc = racine;
    Noeud* contenu_doc = doc->enfants[0];
    Noeud* annexes = doc->enfants[1];

    // DOCUMENT
    afficher_contenu(contenu_doc, LARGEUR_MAX);

    // ANNEXES
    for (int i = 0; i < annexes->nb_enfants; i++) {
        putchar('\n');
        afficher_contenu(annexes->enfants[i]->enfants[0], LARGEUR_MAX);
        printf("|\n");
    }
}
