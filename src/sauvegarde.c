#include "sauvegarde.h"
#include <stdio.h>

#define SAVE_FILE "save.txt"

bool save_progress(const GameContext *ctx) {
    FILE *file = fopen(SAVE_FILE, "w");  // "w" suffit, pas "w+"
    if (!file) {
        perror("Impossible d'ouvrir le fichier de sauvegarde");
        return false;
    }

    fprintf(file, "OperateurName: %s, CurrentLevel: %i SurchargeLevel: %i",
            ctx->operator_name,
            ctx->current_level_idx,
            ctx->surcharge);

    fclose(file);
    return true;
}

bool load_progress(GameContext *ctx) {
    FILE *file = fopen(SAVE_FILE, "r");
    if (!file) {
        perror("Impossible de lire la sauvegarde");
        return false;
    }

    int result = fscanf(file,
        "OperateurName: %49s, CurrentLevel: %i SurchargeLevel: %i",
        ctx->operator_name,    // char[] → déjà un pointeur, pas de &
        &ctx->current_level_idx, 
        &ctx->surcharge);        

    fclose(file);

    if (result == 3) {
        return true;
    } else {
        printf("Erreur : Le fichier de sauvegarde est corrompu ou incomplet.\n");
        return false;
    }
}
