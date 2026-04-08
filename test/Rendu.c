#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Rendu.h"
#include "Arbre.h"

#define LARGEUR_TOTALE 50

typedef struct {
    char ligne[LARGEUR_TOTALE + 2];
    int pos;
    int largeur_texte;
    int profondeur;
    int indent_liste;
} t_contexte;

/* ===================== OUTILS BOÎTES ===================== */

static int largeur_interieure(int profondeur) {
    int w = LARGEUR_TOTALE - 2 * profondeur;
    return (w > 0) ? w : 1;
}

static void prefixe(int profondeur) {
    for (int i = 0; i < profondeur; i++) putchar('|');
}

static void suffixe(int profondeur) {
    for (int i = 0; i < profondeur; i++) putchar('|');
    putchar('\n');
}

static void bordure(int profondeur) {
    for (int i = 0; i < profondeur - 1; i++) putchar('|');
    putchar('+');

    int nb = LARGEUR_TOTALE - 2 - 2 * (profondeur - 1);
    for (int i = 0; i < nb; i++) putchar('-');

    putchar('+');
    for (int i = 0; i < profondeur - 1; i++) putchar('|');
    putchar('\n');
}

/* ===================== LIGNES ===================== */

static void vider(t_contexte* ctx) {
    ctx->pos = 0;
    ctx->ligne[0] = '\0';
}

static void flush(t_contexte* ctx) {
    prefixe(ctx->profondeur);
    printf("%-*.*s", ctx->largeur_texte, ctx->largeur_texte, ctx->ligne);
    suffixe(ctx->profondeur);
    vider(ctx);
}

static void ecrire_mot(t_contexte* ctx, const char* mot) {
    if (!mot || !*mot) return;

    int len = strlen(mot);
    int besoin = (ctx->pos == 0) ? len : len + 1;

    if (ctx->pos > 0 && ctx->pos + besoin > ctx->largeur_texte)
        flush(ctx);

    if (ctx->pos > 0)
        ctx->ligne[ctx->pos++] = ' ';

    int restant = ctx->largeur_texte - ctx->pos;
    int copie = (len <= restant) ? len : restant;

    memcpy(ctx->ligne + ctx->pos, mot, copie);
    ctx->pos += copie;
    ctx->ligne[ctx->pos] = '\0';

    if (copie < len) {
        flush(ctx);
        ecrire_mot(ctx, mot + copie);
    }
}

static void ecrire_mot_maj(t_contexte* ctx, const char* mot) {
    char buf[1024];
    int i;

    for (i = 0; mot[i] && i < 1023; i++)
        buf[i] = toupper((unsigned char)mot[i]);

    buf[i] = '\0';
    ecrire_mot(ctx, buf);
}

/* ===================== TEXTE ===================== */

static void rendre_texte(t_noeud* n, t_contexte* ctx, int maj) {
    if (!n) return;

    if (strcmp(n->type, "mot") == 0) {
        if (maj) ecrire_mot_maj(ctx, n->valeur);
        else     ecrire_mot(ctx, n->valeur);
        return;
    }

    if (strcmp(n->type, "br") == 0) {
        flush(ctx);
        return;
    }

    if (strcmp(n->type, "important") == 0) {
        char buf[1024] = "";
        for (int i = 0; i < n->nb_enfants; i++) {
            if (i) strcat(buf, " ");
            strcat(buf, n->enfants[i]->valeur);
        }

        char imp[1100];
        snprintf(imp, sizeof(imp), "*%s*", buf);

        if (maj) ecrire_mot_maj(ctx, imp);
        else     ecrire_mot(ctx, imp);
        return;
    }

    for (int i = 0; i < n->nb_enfants; i++)
        rendre_texte(n->enfants[i], ctx, maj);
}

/* ===================== LISTES ===================== */

static void rendre_item(t_noeud* n, t_contexte* ctx);

static void rendre_liste(t_noeud* n, t_contexte* ctx) {
    for (int i = 0; i < n->nb_enfants; i++)
        if (strcmp(n->enfants[i]->type, "item") == 0)
            rendre_item(n->enfants[i], ctx);
}

static void rendre_item(t_noeud* n, t_contexte* ctx) {
    flush(ctx);

    int indent = ctx->indent_liste;
    int plen = indent + 2;

    memset(ctx->ligne, ' ', plen);
    ctx->ligne[indent] = '#';
    ctx->ligne[indent + 1] = ' ';
    ctx->pos = plen;
    ctx->ligne[ctx->pos] = '\0';

    for (int i = 0; i < n->nb_enfants; i++)
        rendre_texte(n->enfants[i], ctx, 0);

    flush(ctx);
}

/* ===================== NOEUD ===================== */

static void rendre_noeud(t_noeud* n, t_contexte* ctx) {
    if (!n) return;

    if (strcmp(n->type, "document") == 0) {
        ctx->profondeur = 1;
        ctx->largeur_texte = largeur_interieure(1);

        bordure(1);
        vider(ctx);

        for (int i = 0; i < n->nb_enfants; i++)
            rendre_noeud(n->enfants[i], ctx);

        if (ctx->pos) flush(ctx);
        bordure(1);
        return;
    }

    if (strcmp(n->type, "section") == 0) {
        if (ctx->pos) flush(ctx);

        ctx->profondeur++;
        ctx->largeur_texte = largeur_interieure(ctx->profondeur);

        bordure(ctx->profondeur);
        vider(ctx);

        for (int i = 0; i < n->nb_enfants; i++)
            rendre_noeud(n->enfants[i], ctx);

        if (ctx->pos) flush(ctx);
        bordure(ctx->profondeur);

        ctx->profondeur--;
        ctx->largeur_texte = largeur_interieure(ctx->profondeur);
        return;
    }

    if (strcmp(n->type, "annexe") == 0) {
        ctx->profondeur = 1;
        ctx->largeur_texte = largeur_interieure(1);

        bordure(1);
        vider(ctx);

        for (int i = 0; i < n->nb_enfants; i++)
            rendre_noeud(n->enfants[i], ctx);

        if (ctx->pos) flush(ctx);
        bordure(1);
        return;
    }

    if (strcmp(n->type, "titre") == 0) {
        flush(ctx);
        for (int i = 0; i < n->nb_enfants; i++)
            rendre_texte(n->enfants[i], ctx, 1);
        flush(ctx);
        return;
    }

    if (strcmp(n->type, "liste") == 0) {
        flush(ctx);
        rendre_liste(n, ctx);
        return;
    }

    if (strcmp(n->type, "mot") == 0 ||
        strcmp(n->type, "important") == 0 ||
        strcmp(n->type, "br") == 0) {
        rendre_texte(n, ctx, 0);
        return;
    }

    for (int i = 0; i < n->nb_enfants; i++)
        rendre_noeud(n->enfants[i], ctx);
}

/* ===================== ENTRY POINT ===================== */

void afficher_nanoml(t_noeud* racine) {
    if (!racine) return;

    t_contexte ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.indent_liste = 1;

    for (int i = 0; i < racine->nb_enfants; i++) {
        t_noeud* n = racine->enfants[i];

        if (strcmp(n->type, "document") == 0)
            rendre_noeud(n, &ctx);

        else if (strcmp(n->type, "annexes") == 0) {
            for (int j = 0; j < n->nb_enfants; j++)
                rendre_noeud(n->enfants[j], &ctx);
        }
    }
}