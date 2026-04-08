#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "t_noeud.h"

#define MAX_TAG 64

/* ============================================================
   OUTILS BAS NIVEAU
   ============================================================ */

static void skip_spaces(FILE *f) {
    int c;
    while ((c = fgetc(f)) != EOF) {
        if (!isspace(c)) {
            ungetc(c, f);
            break;
        }
    }
}

static int read_tag(FILE *f, char *tag) {
    int c, i = 0;

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
    int c;

    skip_spaces(f);

    c = fgetc(f);
    if (c != '<') {
        if (c != EOF) ungetc(c, f);
        fseek(f, pos, SEEK_SET);
        return 0;
    }

    if (!read_tag(f, tag)) {
        fseek(f, pos, SEEK_SET);
        return 0;
    }

    fseek(f, pos, SEEK_SET);
    return 1;
}

static int consume_tag(FILE *f, const char *expected) {
    char tag[MAX_TAG];
    int c;

    skip_spaces(f);

    c = fgetc(f);
    if (c != '<') {
        printf("Erreur: attendu <%s> mais trouvé autre chose\n", expected);
        return 0;
    }

    if (!read_tag(f, tag)) {
        printf("Erreur: balise <%s> incomplète\n", expected);
        return 0;
    }

    if (strcmp(tag, expected) != 0) {
        printf("Erreur: attendu <%s> mais trouvé <%s>\n", expected, tag);
        return 0;
    }

    return 1;
}

/* ============================================================
   DÉCLARATIONS
   ============================================================ */

static t_noeud *parse_document(FILE *f);
static t_noeud *parse_annexes(FILE *f);
static t_noeud *parse_annexe(FILE *f);
static t_noeud *parse_contenu(FILE *f);
static t_noeud *parse_section(FILE *f);
static t_noeud *parse_titre(FILE *f);
static t_noeud *parse_liste(FILE *f);
static t_noeud *parse_item(FILE *f);
static t_noeud *parse_item_contenu(FILE *f);
static t_noeud *parse_texte(FILE *f);
static t_noeud *parse_mot_enrichi(FILE *f);
static t_noeud *parse_mot_simple(FILE *f);
static t_noeud *parse_mot_important(FILE *f);

/* ============================================================
   POINT D’ENTRÉE : <texte_enrichi>
   ============================================================ */

t_noeud *parse_texte_enrichi(FILE *f) {
    t_noeud *racine = creer_noeud("texte_enrichi", NULL);

    t_noeud *doc = parse_document(f);
    if (!doc) return NULL;
    ajouter_fils(racine, doc);

    t_noeud *ann = parse_annexes(f);
    if (!ann) return NULL;
    ajouter_fils(racine, ann);

    skip_spaces(f);
    if (fgetc(f) != EOF) {
        printf("Erreur: caractères après les annexes\n");
        return NULL;
    }

    return racine;
}

/* ============================================================
   <document>
   ============================================================ */

static t_noeud *parse_document(FILE *f) {
    if (!consume_tag(f, "document"))
        return NULL;

    t_noeud *n = creer_noeud("document", NULL);

    t_noeud *contenu = parse_contenu(f);
    if (!contenu) return NULL;
    ajouter_fils(n, contenu);

    if (!consume_tag(f, "/document"))
        return NULL;

    return n;
}

/* ============================================================
   <annexes>
   ============================================================ */

static t_noeud *parse_annexes(FILE *f) {
    t_noeud *n = creer_noeud("annexes", NULL);
    char tag[MAX_TAG];

    while (peek_tag(f, tag)) {
        if (strcmp(tag, "annexe") == 0) {
            t_noeud *a = parse_annexe(f);
            if (!a) return NULL;
            ajouter_fils(n, a);
        } else break;
    }

    return n;
}

static t_noeud *parse_annexe(FILE *f) {
    if (!consume_tag(f, "annexe"))
        return NULL;

    t_noeud *n = creer_noeud("annexe", NULL);

    t_noeud *contenu = parse_contenu(f);
    if (!contenu) return NULL;
    ajouter_fils(n, contenu);

    if (!consume_tag(f, "/annexe"))
        return NULL;

    return n;
}

/* ============================================================
   <contenu>
   ============================================================ */

static t_noeud *parse_contenu(FILE *f) {
    t_noeud *n = creer_noeud("contenu", NULL);
    char tag[MAX_TAG];

    while (1) {
        skip_spaces(f);

        long pos = ftell(f);

        if (!peek_tag(f, tag)) {
            t_noeud *m = parse_mot_enrichi(f);
            if (!m) {
                fseek(f, pos, SEEK_SET);
                return n;
            }
            ajouter_fils(n, m);
            continue;
        }

        if (tag[0] == '/') return n;

        if (strcmp(tag, "section") == 0) {
            t_noeud *s = parse_section(f);
            if (!s) return NULL;
            ajouter_fils(n, s);
        }
        else if (strcmp(tag, "titre") == 0) {
            t_noeud *t = parse_titre(f);
            if (!t) return NULL;
            ajouter_fils(n, t);
        }
        else if (strcmp(tag, "liste") == 0) {
            t_noeud *l = parse_liste(f);
            if (!l) return NULL;
            ajouter_fils(n, l);
        }
        else if (strcmp(tag, "important") == 0 || strcmp(tag, "br/") == 0) {
            t_noeud *m = parse_mot_enrichi(f);
            if (!m) return NULL;
            ajouter_fils(n, m);
        }
        else {
            printf("Erreur: balise <%s> interdite dans <contenu>\n", tag);
            return NULL;
        }
    }
}

/* ============================================================
   <section>
   ============================================================ */

static t_noeud *parse_section(FILE *f) {
    if (!consume_tag(f, "section"))
        return NULL;

    t_noeud *n = creer_noeud("section", NULL);

    t_noeud *c = parse_contenu(f);
    if (!c) return NULL;
    ajouter_fils(n, c);

    if (!consume_tag(f, "/section"))
        return NULL;

    return n;
}

/* ============================================================
   <titre>
   ============================================================ */

static t_noeud *parse_titre(FILE *f) {
    if (!consume_tag(f, "titre"))
        return NULL;

    t_noeud *t = creer_noeud("titre", NULL);

    t_noeud *txt = parse_texte(f);
    if (!txt) return NULL;
    ajouter_fils(t, txt);

    if (!consume_tag(f, "/titre"))
        return NULL;

    return t;
}

/* ============================================================
   <liste> et <item>
   ============================================================ */

static t_noeud *parse_liste(FILE *f) {
    if (!consume_tag(f, "liste"))
        return NULL;

    t_noeud *l = creer_noeud("liste", NULL);
    char tag[MAX_TAG];

    while (peek_tag(f, tag) && strcmp(tag, "item") == 0) {
        t_noeud *it = parse_item(f);
        if (!it) return NULL;
        ajouter_fils(l, it);
    }

    if (!consume_tag(f, "/liste"))
        return NULL;

    return l;
}

static t_noeud *parse_item(FILE *f) {
    if (!consume_tag(f, "item"))
        return NULL;

    t_noeud *i = creer_noeud("item", NULL);

    t_noeud *c = parse_item_contenu(f);
    if (!c) return NULL;
    ajouter_fils(i, c);

    if (!consume_tag(f, "/item"))
        return NULL;

    return i;
}

static t_noeud *parse_item_contenu(FILE *f) {
    t_noeud *n = creer_noeud("item_contenu", NULL);
    char tag[MAX_TAG];

    while (1) {
        skip_spaces(f);

        if (peek_tag(f, tag) && strcmp(tag, "/item") == 0)
            break;

        if (peek_tag(f, tag) && strcmp(tag, "liste") == 0) {
            t_noeud *l = parse_liste(f);
            if (!l) return NULL;
            ajouter_fils(n, l);
            continue;
        }

        t_noeud *t = parse_texte(f);
        if (!t) break;
        ajouter_fils(n, t);
    }

    return n;
}

/* ============================================================
   <texte> et mots
   ============================================================ */

static t_noeud *parse_texte(FILE *f) {
    t_noeud *n = creer_noeud("texte", NULL);

    while (1) {
        long pos = ftell(f);
        t_noeud *m = parse_mot_enrichi(f);
        if (!m) {
            fseek(f, pos, SEEK_SET);
            break;
        }
        ajouter_fils(n, m);
    }

    return n;
}

static t_noeud *parse_mot_enrichi(FILE *f) {
    char tag[MAX_TAG];
    long pos;

    skip_spaces(f);
    pos = ftell(f);

    if (peek_tag(f, tag)) {
        if (strcmp(tag, "important") == 0)
            return parse_mot_important(f);

        if (strcmp(tag, "br/") == 0) {
            consume_tag(f, "br/");
            return creer_noeud("br", NULL);
        }

        return NULL;
    }

    return parse_mot_simple(f);
}

static t_noeud *parse_mot_simple(FILE *f) {
    skip_spaces(f);

    char buf[256];
    int i = 0;
    int c;

    while ((c = fgetc(f)) != EOF) {
        if (c == '<') {
            ungetc(c, f);
            break;
        }
        buf[i++] = (char)c;
    }

    if (i == 0) return NULL;

    buf[i] = '\0';
    return creer_noeud("mot_simple", buf);
}

static t_noeud *parse_mot_important(FILE *f) {
    if (!consume_tag(f, "important"))
        return NULL;

    t_noeud *imp = creer_noeud("important", NULL);

    while (1) {
        long pos = ftell(f);
        t_noeud *m = parse_mot_simple(f);
        if (!m) {
            fseek(f, pos, SEEK_SET);
            break;
        }
        ajouter_fils(imp, m);
    }

    if (!consume_tag(f, "/important"))
        return NULL;

    return imp;
}
