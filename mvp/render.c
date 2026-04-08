#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "render.h"
#include "t_noeud.h"

#define DEBUG 1

/* =========================
   DEBUG
   ========================= */

static void debug_noeud(t_noeud *n, int indent) {
#if DEBUG
    for (int i = 0; i < indent; i++) printf("  ");
    if (!n) {
        printf("[DEBUG] (NULL)\n");
        return;
    }
    printf("[DEBUG] type='%s' nb_fils=%d", n->type, n->nb_fils);
    if (n->texte) printf(" texte='%s'", n->texte);
    printf("\n");
#endif
}

/* =========================
   OUTILS D’AFFICHAGE
   ========================= */

static void print_top(int width) {
    putchar('+');
    for (int i = 0; i < width - 2; i++) putchar('-');
    putchar('+');
    putchar('\n');
}

static void print_bottom(int width) {
    print_top(width);
}

static void print_line(const char *text, int width) {
    int len = strlen(text);
    if (len > width - 2) len = width - 2;

    putchar('|');
    fwrite(text, 1, len, stdout);
    for (int i = 0; i < width - 2 - len; i++) putchar(' ');
    putchar('|');
    putchar('\n');
}

static void wrap_text(const char *text, int width) {
    int max = width - 2;
    int len = strlen(text);
    int pos = 0;

    while (pos < len) {
        int chunk = (len - pos > max) ? max : len - pos;
        char buf[512];
        strncpy(buf, text + pos, chunk);
        buf[chunk] = '\0';
        print_line(buf, width);
        pos += chunk;
    }
}

/* =========================
   RENDU SPÉCIFIQUE
   ========================= */

static void render_noeud(t_noeud *n, int width, int indent);

static void render_titre(t_noeud *n, int width) {
    debug_noeud(n, 1);

    if (!n || n->nb_fils == 0 || !n->fils[0]) return;

    t_noeud *texte = n->fils[0];
    char buffer[512] = "";
    int first = 1;

    for (int i = 0; i < texte->nb_fils; i++) {
        t_noeud *m = texte->fils[i];

        if (!m) continue;

        if (strcmp(m->type, "mot_simple") == 0 && m->texte) {
            if (!first) strcat(buffer, " ");
            strcat(buffer, m->texte);
            first = 0;
        }
        else if (strcmp(m->type, "important") == 0) {
            for (int j = 0; j < m->nb_fils; j++) {
                t_noeud *ms = m->fils[j];
                if (ms && strcmp(ms->type, "mot_simple") == 0 && ms->texte) {
                    if (!first) strcat(buffer, " ");
                    strcat(buffer, ms->texte);
                    first = 0;
                }
            }
        }
    }

    for (int k = 0; buffer[k]; k++)
        buffer[k] = toupper(buffer[k]);

    print_line("", width);
    wrap_text(buffer, width);
    print_line("", width);
}

static void render_important(t_noeud *n, int width) {
    debug_noeud(n, 1);

    char buf[512] = "";
    int first = 1;

    for (int i = 0; i < n->nb_fils; i++) {
        t_noeud *m = n->fils[i];
        if (m && strcmp(m->type, "mot_simple") == 0 && m->texte) {
            if (!first) strcat(buf, " ");
            strcat(buf, m->texte);
            first = 0;
        }
    }

    for (int k = 0; buf[k]; k++)
        buf[k] = toupper(buf[k]);

    wrap_text(buf, width);
}

static void render_liste(t_noeud *n, int width, int indent);

static void render_item(t_noeud *item, int width, int indent) {
    debug_noeud(item, indent);

    if (!item || item->nb_fils == 0 || !item->fils[0]) return;

    t_noeud *contenu = item->fils[0];

    char prefix[32];
    memset(prefix, ' ', indent * 2);
    prefix[indent * 2] = '\0';

    char line[512];
    strcpy(line, prefix);
    strcat(line, "# ");

    int first = 1;

    for (int i = 0; i < contenu->nb_fils; i++) {
        t_noeud *c = contenu->fils[i];
        if (!c) continue;

        if (strcmp(c->type, "texte") == 0) {
            for (int j = 0; j < c->nb_fils; j++) {
                t_noeud *m = c->fils[j];
                if (!m) continue;

                if (strcmp(m->type, "mot_simple") == 0 && m->texte) {
                    if (!first) strcat(line, " ");
                    strcat(line, m->texte);
                    first = 0;
                }
                else if (strcmp(m->type, "important") == 0) {
                    char imp[256] = "";
                    for (int k = 0; k < m->nb_fils; k++) {
                        t_noeud *ms = m->fils[k];
                        if (ms && strcmp(ms->type, "mot_simple") == 0 && ms->texte) {
                            if (strlen(imp) > 0) strcat(imp, " ");
                            strcat(imp, ms->texte);
                        }
                    }
                    for (int k = 0; imp[k]; k++)
                        imp[k] = toupper(imp[k]);

                    if (!first) strcat(line, " ");
                    strcat(line, imp);
                    first = 0;
                }
            }
        }
    }

    wrap_text(line, width);

    for (int i = 0; i < contenu->nb_fils; i++) {
        t_noeud *c = contenu->fils[i];
        if (c && strcmp(c->type, "liste") == 0)
            render_liste(c, width, indent + 1);
    }
}

static void render_liste(t_noeud *n, int width, int indent) {
    debug_noeud(n, indent);

    for (int i = 0; i < n->nb_fils; i++) {
        t_noeud *item = n->fils[i];
        if (item && strcmp(item->type, "item") == 0)
            render_item(item, width, indent);
    }
}

/* =========================
   RENDU GÉNÉRAL
   ========================= */

static void render_noeud(t_noeud *n, int width, int indent) {
    debug_noeud(n, indent);
    if (!n) return;

    if (strcmp(n->type, "texte_enrichi") == 0) {
        if (n->nb_fils >= 1 && n->fils[0])
            render_noeud(n->fils[0], 50, 0);

        printf("|\n");

        if (n->nb_fils >= 2 && n->fils[1]) {
            t_noeud *ann = n->fils[1];
            for (int i = 0; i < ann->nb_fils; i++) {
                render_noeud(ann->fils[i], 50, 0);
                printf("|\n");
            }
        }
        return;
    }

    if (strcmp(n->type, "document") == 0 ||
        strcmp(n->type, "annexe") == 0 ||
        strcmp(n->type, "section") == 0) {

        print_top(width);
        if (n->nb_fils > 0 && n->fils[0])
            render_noeud(n->fils[0], width - 2, indent);
        print_bottom(width);
        return;
    }

    if (strcmp(n->type, "contenu") == 0) {
        for (int i = 0; i < n->nb_fils; i++)
            render_noeud(n->fils[i], width, indent);
        return;
    }

    if (strcmp(n->type, "titre") == 0) {
        render_titre(n, width);
        return;
    }

    if (strcmp(n->type, "texte") == 0) {
        for (int i = 0; i < n->nb_fils; i++)
            render_noeud(n->fils[i], width, indent);
        return;
    }

    if (strcmp(n->type, "mot_simple") == 0) {
        if (n->texte) wrap_text(n->texte, width);
        return;
    }

    if (strcmp(n->type, "important") == 0) {
        render_important(n, width);
        return;
    }

    if (strcmp(n->type, "br") == 0) {
        print_line("", width);
        return;
    }

    if (strcmp(n->type, "liste") == 0) {
        render_liste(n, width, indent);
        return;
    }

    if (strcmp(n->type, "item") == 0) {
        render_item(n, width, indent);
        return;
    }

    if (strcmp(n->type, "item_contenu") == 0) {
        for (int i = 0; i < n->nb_fils; i++)
            render_noeud(n->fils[i], width, indent);
        return;
    }
}

void render(t_noeud *racine) {
#if DEBUG
    printf("[DEBUG] Début du rendu\n");
#endif
    render_noeud(racine, 50, 0);
}
