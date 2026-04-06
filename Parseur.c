#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Parseur.h"
#include "Arbre.h"

void lire_caractere(t_Parseur* parseur) {
    int c = fgetc(parseur->fichier);
    parseur->caractere = (c == EOF ? '\0' : (char)c);
}

int est_en_fin(const t_Parseur* parseur) {
    return parseur->caractere == '\0';
}

void amorcer(t_Parseur* parseur, const char* nom_fichier) {
    parseur->fichier = fopen(nom_fichier, "r");
    if (!parseur->fichier) {
        fprintf(stderr, "Erreur ouverture %s\n", nom_fichier);
        exit(EXIT_FAILURE);
    }
    lire_caractere(parseur);
}

void terminer(t_Parseur* parseur) {
    if (parseur->fichier) {
        fclose(parseur->fichier);
        parseur->fichier = NULL;
    }
}

void sauter_espaces(t_Parseur* parseur) {
    while (!est_en_fin(parseur) && isspace((unsigned char)parseur->caractere))
        lire_caractere(parseur);
}

void consommer(t_Parseur* parseur, char attendu) {
    if (parseur->caractere != attendu) {
        fprintf(stderr, "Erreur : '%c' lu, '%c' attendu\n",
                parseur->caractere, attendu);
        exit(EXIT_FAILURE);
    }
    lire_caractere(parseur);
}

void consommer_chaine(t_Parseur* parseur, const char* attendu) {
    int index = 0;
    while (attendu[index] != '\0') {
        if (parseur->caractere != attendu[index]) {
            fprintf(stderr, "Erreur en lisant \"%s\"\n", attendu);
            exit(EXIT_FAILURE);
        }
        lire_caractere(parseur);
        index++;
    }
}

int commence_par(t_Parseur* parseur, const char* chaine) {
    int len = strlen(chaine);
    char buffer[64];
    int index = 0;

    while (index < len && !est_en_fin(parseur)) {
        buffer[index] = parseur->caractere;
        lire_caractere(parseur);
        index++;
    }

    if (index < len) {
        while (index > 0) {
            index--;
            ungetc(buffer[index], parseur->fichier);
        }
        parseur->caractere = buffer[0];
        return 0;
    }

    int ok = 1;
    for (int i = 0; i < len; i++) {
        if (buffer[i] != chaine[i]) {
            ok = 0;
            break;
        }
    }

    for (int i = len - 1; i >= 0; i--)
        ungetc(buffer[i], parseur->fichier);

    parseur->caractere = buffer[0];
    return ok;
}
t_noeud* texte_enrichi(t_Parseur* parseur) {
    sauter_espaces(parseur);

    t_noeud* noeud_document = document(parseur);

    sauter_espaces(parseur);

    t_noeud* noeud_annexes = annexes(parseur);

    t_noeud* racine = creer_noeud("texte_enrichi", NULL);
    ajouter_enfant(racine, noeud_document);

    if (noeud_annexes)
        ajouter_enfant(racine, noeud_annexes);

    return racine;
}

t_noeud* document(t_Parseur* parseur) {
    sauter_espaces(parseur);
    consommer_chaine(parseur, "<document>");
    sauter_espaces(parseur);

    t_noeud* contenu_doc = contenu(parseur);

    sauter_espaces(parseur);
    consommer_chaine(parseur, "</document>");

    t_noeud* noeud = creer_noeud("document", NULL);
    if (contenu_doc) ajouter_enfant(noeud, contenu_doc);

    return noeud;
}

t_noeud* annexes(t_Parseur* parseur) {
    sauter_espaces(parseur);

    t_noeud* noeud = creer_noeud("annexes", NULL);

    while (commence_par(parseur, "<annexe>")) {
        t_noeud* a = annexe(parseur);
        ajouter_enfant(noeud, a);
        sauter_espaces(parseur);
    }

    if (noeud->nb_enfants == 0) {
        liberer_arbre(noeud);
        return NULL;
    }

    return noeud;
}

t_noeud* annexe(t_Parseur* parseur) {
    consommer_chaine(parseur, "<annexe>");
    sauter_espaces(parseur);

    t_noeud* contenu_annexe = contenu(parseur);

    sauter_espaces(parseur);
    consommer_chaine(parseur, "</annexe>");

    t_noeud* noeud = creer_noeud("annexe", NULL);
    if (contenu_annexe) ajouter_enfant(noeud, contenu_annexe);

    return noeud;
}

t_noeud* contenu(t_Parseur* parseur) {
    t_noeud* noeud = creer_noeud("contenu", NULL);
    sauter_espaces(parseur);

    while (!est_en_fin(parseur)) {
        t_noeud* element = NULL;

        if (commence_par(parseur, "<section>")) element = section(parseur);
        else if (commence_par(parseur, "<titre>")) element = titre(parseur);
        else if (commence_par(parseur, "<liste>")) element = liste(parseur);
        else if (!commence_par(parseur, "</document>") &&
                 !commence_par(parseur, "</section>") &&
                 !commence_par(parseur, "</annexe>")) {
            element = mot_enrichi(parseur);
        } else {
            break;
        }

        if (element) ajouter_enfant(noeud, element);
        sauter_espaces(parseur);
    }

    if (noeud->nb_enfants == 0) {
        liberer_arbre(noeud);
        return NULL;
    }

    return noeud;
}

t_noeud* section(t_Parseur* parseur) {
    consommer_chaine(parseur, "<section>");
    sauter_espaces(parseur);

    t_noeud* contenu_section = contenu(parseur);

    sauter_espaces(parseur);
    consommer_chaine(parseur, "</section>");

    t_noeud* noeud = creer_noeud("section", NULL);
    if (contenu_section) ajouter_enfant(noeud, contenu_section);

    return noeud;
}

t_noeud* titre(t_Parseur* parseur) {
    consommer_chaine(parseur, "<titre>");
    sauter_espaces(parseur);

    t_noeud* texte_titre = texte(parseur);

    sauter_espaces(parseur);
    consommer_chaine(parseur, "</titre>");

    t_noeud* noeud = creer_noeud("titre", NULL);
    if (texte_titre) ajouter_enfant(noeud, texte_titre);

    return noeud;
}

t_noeud* liste(t_Parseur* parseur) {
    consommer_chaine(parseur, "<liste>");
    sauter_espaces(parseur);

    t_noeud* noeud = creer_noeud("liste", NULL);

    while (commence_par(parseur, "<item>")) {
        t_noeud* it = item(parseur);
        ajouter_enfant(noeud, it);
        sauter_espaces(parseur);
    }

    consommer_chaine(parseur, "</liste>");
    return noeud;
}

t_noeud* item(t_Parseur* parseur) {
    consommer_chaine(parseur, "<item>");
    sauter_espaces(parseur);

    t_noeud* contenu_item = NULL;

    if (commence_par(parseur, "<liste>"))
        contenu_item = liste_texte(parseur);
    else
        contenu_item = texte_liste(parseur);

    sauter_espaces(parseur);
    consommer_chaine(parseur, "</item>");

    t_noeud* noeud = creer_noeud("item", NULL);
    if (contenu_item) ajouter_enfant(noeud, contenu_item);

    return noeud;
}

t_noeud* liste_texte(t_Parseur* parseur) {
    t_noeud* noeud = creer_noeud("liste_texte", NULL);

    t_noeud* l = liste(parseur);
    ajouter_enfant(noeud, l);

    sauter_espaces(parseur);

    if (!commence_par(parseur, "</item>")) {
        t_noeud* tl = texte_liste(parseur);
        if (tl) ajouter_enfant(noeud, tl);
    }

    return noeud;
}

t_noeud* texte_liste(t_Parseur* parseur) {
    t_noeud* noeud = creer_noeud("texte_liste", NULL);

    t_noeud* t = texte(parseur);
    if (t) ajouter_enfant(noeud, t);

    sauter_espaces(parseur);

    if (!commence_par(parseur, "</item>")) {
        t_noeud* lt = liste_texte(parseur);
        if (lt) ajouter_enfant(noeud, lt);
    }

    return noeud;
}

t_noeud* texte(t_Parseur* parseur) {
    t_noeud* noeud = creer_noeud("texte", NULL);
    sauter_espaces(parseur);

    while (!est_en_fin(parseur) &&
           !commence_par(parseur, "</titre>") &&
           !commence_par(parseur, "</section>") &&
           !commence_par(parseur, "</item>") &&
           !commence_par(parseur, "</liste>") &&
           !commence_par(parseur, "</document>") &&
           !commence_par(parseur, "</annexe>")) {

        t_noeud* mot = mot_enrichi(parseur);
        if (mot) ajouter_enfant(noeud, mot);

        sauter_espaces(parseur);
    }

    if (noeud->nb_enfants == 0) {
        liberer_arbre(noeud);
        return NULL;
    }

    return noeud;
}

t_noeud* mot_enrichi(t_Parseur* parseur) {
    if (commence_par(parseur, "<important>")) return mot_important(parseur);
    if (commence_par(parseur, "<br/>")) return balise_br(parseur);
    return mot_simple(parseur);
}

t_noeud* mot_simple(t_Parseur* parseur) {
    char mot[1024];
    int index = 0;

    while (!est_en_fin(parseur) &&
           !isspace((unsigned char)parseur->caractere) &&
           parseur->caractere != '<' &&
           index < (int)sizeof(mot) - 1) {

        mot[index++] = parseur->caractere;
        lire_caractere(parseur);
    }

    mot[index] = '\0';

    if (index == 0) return NULL;

    return creer_noeud("mot", mot);
}

t_noeud* mot_important(t_Parseur* parseur) {
    consommer_chaine(parseur, "<important>");
    sauter_espaces(parseur);

    t_noeud* noeud = creer_noeud("important", NULL);

    while (!commence_par(parseur, "</important>")) {
        t_noeud* mot = mot_simple(parseur);
        if (!mot) break;
        ajouter_enfant(noeud, mot);
        sauter_espaces(parseur);
    }

    consommer_chaine(parseur, "</important>");
    return noeud;
}

t_noeud* balise_br(t_Parseur* parseur) {
    consommer_chaine(parseur, "<br/>");
    return creer_noeud("br", NULL);
}
