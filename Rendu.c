#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "Rendu.h"

static void afficher_noeud(t_noeud* noeud, int indent);

static void indent(int n) {
    for (int i = 0; i < n; i++)
        putchar(' ');
}

static void afficher_texte_maj(const char* texte) {
    for (int i = 0; texte[i]; i++)
        putchar((char)toupper((unsigned char)texte[i]));
}

static void afficher_noeud(t_noeud* noeud, int indent_level) {
    if (!noeud) return;

    if (strcmp(noeud->type, "document") == 0 ||
        strcmp(noeud->type, "annexe") == 0 ||
        strcmp(noeud->type, "section") == 0 ||
        strcmp(noeud->type, "contenu") == 0 ||
        strcmp(noeud->type, "annexes") == 0 ||
        strcmp(noeud->type, "texte_enrichi") == 0) {

        for (int i = 0; i < noeud->nb_enfants; i++)
            afficher_noeud(noeud->enfants[i], indent_level);
    }
    else if (strcmp(noeud->type, "titre") == 0) {
        putchar('\n');

        for (int i = 0; i < noeud->nb_enfants; i++) {
            t_noeud* texte = noeud->enfants[i];

            if (strcmp(texte->type, "texte") == 0) {
                for (int j = 0; j < texte->nb_enfants; j++) {
                    t_noeud* mot = texte->enfants[j];

                    if (strcmp(mot->type, "mot") == 0) {
                        afficher_texte_maj(mot->valeur);
                        putchar(' ');
                    }
                }
            }
        }

        putchar('\n');
    }
    else if (strcmp(noeud->type, "liste") == 0) {
        for (int i = 0; i < noeud->nb_enfants; i++)
            afficher_noeud(noeud->enfants[i], indent_level + 2);
    }
    else if (strcmp(noeud->type, "item") == 0) {
        indent(indent_level);
        printf("# ");

        for (int i = 0; i < noeud->nb_enfants; i++)
            afficher_noeud(noeud->enfants[i], indent_level + 2);

        putchar('\n');
    }
    else if (strcmp(noeud->type, "texte") == 0) {
        for (int i = 0; i < noeud->nb_enfants; i++)
            afficher_noeud(noeud->enfants[i], indent_level);
    }
    else if (strcmp(noeud->type, "mot") == 0) {
        printf("%s ", noeud->valeur ? noeud->valeur : "");
    }
    else if (strcmp(noeud->type, "important") == 0) {
        putchar('*');

        for (int i = 0; i < noeud->nb_enfants; i++)
            afficher_noeud(noeud->enfants[i], indent_level);

        putchar('*');
    }
    else if (strcmp(noeud->type, "br") == 0) {
        putchar('\n');
    }
}

void afficher_nanoml(t_noeud* racine) {
    afficher_noeud(racine, 0);
    putchar('\n');
}
