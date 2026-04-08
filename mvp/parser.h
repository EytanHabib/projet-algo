#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "t_noeud.h"

/* Analyse complète : <document> <annexes> */
t_noeud *parse_texte_enrichi(FILE *f);

#endif
