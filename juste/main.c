#include <stdio.h>
#include "parser.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s fichier.nanoml\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("Erreur ouverture");
        return 1;
    }

    if (parse_texte_enrichi(f))
        printf("Document NanoML VALIDE ✅\n");
    else
        printf("Document NanoML INVALIDE ❌\n");

    fclose(f);
    return 0;
}
