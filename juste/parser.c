#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

#define MAX_TAG 128

/* ============================================================
   OUTILS BAS NIVEAU
   ============================================================ */

static int peek_char(FILE *f) {
    int c = fgetc(f);
    if (c == EOF) return EOF;
    ungetc(c, f);
    return c;
}

static void skip_blanks(FILE *f) {
    int c;
    while ((c = fgetc(f)) != EOF) {
        if (!isspace(c)) {
            ungetc(c, f);
            return;
        }
    }
}

static int read_tag(FILE *f, char *tag) {
    int c = fgetc(f);
    if (c != '<') return 0;

    int i = 0;
    while ((c = fgetc(f)) != EOF && c != '>') {
        if (i < MAX_TAG - 1)
            tag[i++] = (char)c;
    }
    if (c != '>') return 0;

    tag[i] = '\0';
    return 1;
}

static int peek_tag(FILE *f, char *tag) {
    long pos = ftell(f);
    int c = fgetc(f);

    if (c != '<') {
        fseek(f, pos, SEEK_SET);
        return 0;
    }

    int i = 0;
    while ((c = fgetc(f)) != EOF && c != '>') {
        if (i < MAX_TAG - 1)
            tag[i++] = (char)c;
    }
    if (c != '>') {
        fseek(f, pos, SEEK_SET);
        return 0;
    }

    tag[i] = '\0';
    fseek(f, pos, SEEK_SET);
    return 1;
}

static int consume_tag(FILE *f, const char *expected) {
    skip_blanks(f);

    char tag[MAX_TAG];
    if (!read_tag(f, tag)) return 0;

    if (strcmp(tag, expected) != 0) {
        printf("Erreur: attendu <%s> mais trouve <%s>\n", expected, tag);
        return 0;
    }
    return 1;
}

/* ============================================================
   DECLARATIONS
   ============================================================ */

static int parse_document(FILE *f);
static int parse_annexes(FILE *f);
static int parse_annexe(FILE *f);
static int parse_contenu(FILE *f);
static int parse_section(FILE *f);
static int parse_titre(FILE *f);
static int parse_liste(FILE *f);
static int parse_item(FILE *f);
static int parse_liste_texte(FILE *f);
static int parse_texte_liste(FILE *f);
static int parse_texte(FILE *f, const char *stop1, const char *stop2);
static int parse_mot_enrichi(FILE *f);
static int parse_mot_simple(FILE *f);
static int parse_mot_important(FILE *f);

/* ============================================================
   MOTS / TEXTE
   ============================================================ */

static int parse_mot_simple(FILE *f) {
    int c, seen = 0;

    while ((c = fgetc(f)) != EOF) {
        if (c == '<') {
            ungetc(c, f);
            break;
        }
        seen = 1;
    }
    return seen;
}

static int parse_mot_important(FILE *f) {

    if (!consume_tag(f, "important"))
        return 0;

    if (!parse_mot_simple(f))
        return 0;

    while (1) {
        int c = peek_char(f);
        if (c == EOF) break;

        if (c == '<') {
            char tag[MAX_TAG];
            peek_tag(f, tag);
            if (strcmp(tag, "/important") == 0)
                break;
        }
        if (!parse_mot_simple(f))
            return 0;
    }

    return consume_tag(f, "/important");
}

static int parse_mot_enrichi(FILE *f) {
    int c = peek_char(f);
    if (c == EOF) return 0;

    if (c == '<') {
        char tag[MAX_TAG];
        if (!peek_tag(f, tag)) return 0;

        if (strcmp(tag, "important") == 0)
            return parse_mot_important(f);

        if (strcmp(tag, "br/") == 0)
            return consume_tag(f, "br/");

        return 0;
    }

    return parse_mot_simple(f);
}

static int parse_texte(FILE *f, const char *stop1, const char *stop2) {
    int ok = 0;

    while (1) {
        int c = peek_char(f);
        if (c == EOF) break;

        if (c == '<') {
            char tag[MAX_TAG];
            peek_tag(f, tag);

            if ((stop1 && strcmp(tag, stop1) == 0) ||
                (stop2 && strcmp(tag, stop2) == 0))
                break;

            if (strcmp(tag, "section") == 0 ||
                strcmp(tag, "/section") == 0 ||
                strcmp(tag, "liste") == 0 ||
                strcmp(tag, "/liste") == 0 ||
                strcmp(tag, "item") == 0 ||
                strcmp(tag, "/item") == 0 ||
                strcmp(tag, "document") == 0 ||
                strcmp(tag, "/document") == 0 ||
                strcmp(tag, "annexe") == 0 ||
                strcmp(tag, "/annexe") == 0 ||
                strcmp(tag, "titre") == 0 ||
                strcmp(tag, "/titre") == 0)
                break;
        }

        if (!parse_mot_enrichi(f))
            return ok;

        ok = 1;
    }

    return ok;
}

/* ============================================================
   TITRE
   ============================================================ */

static int parse_titre(FILE *f) {

    if (!consume_tag(f, "titre"))
        return 0;

    if (!parse_texte(f, "/titre", NULL))
        return 0;

    return consume_tag(f, "/titre");
}

/* ============================================================
   LISTES
   ============================================================ */

static int parse_liste(FILE *f) {

    if (!consume_tag(f, "liste"))
        return 0;

    while (1) {
        int c = peek_char(f);
        if (c == EOF) return 0;

        if (c == '<') {
            char tag[MAX_TAG];
            peek_tag(f, tag);

            if (strcmp(tag, "item") == 0) {
                if (!parse_item(f)) return 0;
            }
            else if (strcmp(tag, "/liste") == 0) {
                break;
            }
            else return 0;
        }
        else return 0;
    }

    return consume_tag(f, "/liste");
}

static int parse_liste_texte(FILE *f) {
    int c = peek_char(f);
    if (c == EOF) return 1;

    if (c == '<') {
        char tag[MAX_TAG];
        peek_tag(f, tag);

        if (strcmp(tag, "liste") == 0) {
            if (!parse_liste(f)) return 0;
            return parse_texte_liste(f);
        }
    }

    return 1;
}

static int parse_texte_liste(FILE *f) {
    long pos = ftell(f);

    if (!parse_texte(f, "liste", "/item")) {
        fseek(f, pos, SEEK_SET);
        return 1;
    }

    return parse_liste_texte(f);
}

static int parse_item(FILE *f) {

    if (!consume_tag(f, "item"))
        return 0;

    char tag[MAX_TAG];
    int c = peek_char(f);

    if (c == '<') {
        peek_tag(f, tag);

        if (strcmp(tag, "liste") == 0) {
            if (!parse_liste_texte(f)) return 0;
        }
        else {
            if (!parse_texte_liste(f)) return 0;
        }
    }
    else {
        if (!parse_texte_liste(f)) return 0;
    }

    return consume_tag(f, "/item");
}

/* ============================================================
   SECTION / CONTENU
   ============================================================ */

static int parse_section(FILE *f) {

    if (!consume_tag(f, "section"))
        return 0;

    if (!parse_contenu(f))
        return 0;

    return consume_tag(f, "/section");
}

static int parse_contenu(FILE *f) {

    while (1) {
        int c = peek_char(f);
        if (c == EOF) return 1;

        if (c == '<') {
            char tag[MAX_TAG];
            peek_tag(f, tag);

            if (tag[0] == '/') return 1;

            if (strcmp(tag, "section") == 0) {
                if (!parse_section(f)) return 0;
            }
            else if (strcmp(tag, "titre") == 0) {
                if (!parse_titre(f)) return 0;
            }
            else if (strcmp(tag, "liste") == 0) {
                if (!parse_liste(f)) return 0;
            }
            else if (strcmp(tag, "important") == 0 ||
                     strcmp(tag, "br/") == 0) {
                if (!parse_mot_enrichi(f)) return 0;
            }
            else return 0;
        }
        else {
            if (!parse_mot_enrichi(f)) return 0;
        }
    }
}

/* ============================================================
   DOCUMENT / ANNEXES
   ============================================================ */

static int parse_document(FILE *f) {

    if (!consume_tag(f, "document"))
        return 0;

    if (!parse_contenu(f))
        return 0;

    return consume_tag(f, "/document");
}

static int parse_annexe(FILE *f) {

    if (!consume_tag(f, "annexe"))
        return 0;

    if (!parse_contenu(f))
        return 0;

    return consume_tag(f, "/annexe");
}

static int parse_annexes(FILE *f) {

    while (1) {
        int c = peek_char(f);
        if (c == EOF) return 1;

        if (c == '<') {
            char tag[MAX_TAG];
            peek_tag(f, tag);

            if (strcmp(tag, "annexe") == 0) {
                if (!parse_annexe(f)) return 0;
            }
            else return 1;
        }
        else {
            c = fgetc(f);
            if (!isspace(c)) return 0;
        }
    }
}

/* ============================================================
   RACINE
   ============================================================ */

int parse_texte_enrichi(FILE *f) {

    skip_blanks(f);

    if (!parse_document(f))
        return 0;

    if (!parse_annexes(f))
        return 0;

    int c;
    while ((c = fgetc(f)) != EOF) {
        if (!isspace(c)) {
            printf("Erreur: texte après la fin du document\n");
            return 0;
        }
    }

    return 1;
}
