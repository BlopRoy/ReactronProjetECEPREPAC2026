#include "sauvegarde.h"
#include <stdio.h>

#define SAVE_FILE "save.txt"



bool save_progress(const GameContext *ctx) {
    FILE *file = fopen(SAVE_FILE,"w+");
    if (!file) {
        perror("File could'not open !!!");
        return false;
    }

    //Ecriture des info dans le fichier
    fprintf(file,"OperateurName: %s, CurrentLevel: %i SurchargeLevel: %i",ctx->operator_name, ctx->current_level_idx, ctx->surcharge);
    
    fclose(file);
    return true;
}

bool load_progress(GameContext *ctx) {
    FILE *file = fopen(SAVE_FILE, "r");
    if (!file) {
        perror("Impossible de lire la sauvegarde");
        return false;
    }

    // fscanf va chercher le texte exact et extraire les valeurs (%s et %i)
    // Le %49s limite la lecture pour éviter un dépassement de mémoire (Buffer Overflow)
    int result = fscanf(file, "OperateurName: %49s, CurrentLevel: %i SurchargeLevel: %i",ctx->operator_name, ctx->current_level_idx, ctx->surcharge);
    fclose(file);

    // fscanf doit avoir trouvé et rempli nos 3 variables avec succès
    if (result == 3) {
        return true;
    } else {
        printf("Erreur : Le fichier de sauvegarde est corrompu ou incomplet.\n");
        return false;
    }
}

/*
bool load_progress(GameContext *ctx) {
    FILE *file = fopen(SAVE_FILE, "rb");
    if (!file) return false;

    size_t read_bytes = 0;
    read_bytes += fread(ctx->operator_name, sizeof(char), 50, file);
    read_bytes += fread(&ctx->current_level_idx, sizeof(int), 1, file);
    read_bytes += fread(&ctx->surcharge, sizeof(int), 1, file);

    fclose(file);
    
    // Validation rapide que des données valides ont été lues
    if (read_bytes == 0) return false;
    return true;
}


bool save_progress(const GameContext *ctx) {
    FILE *file = fopen(SAVE_FILE, "wb");
    if (!file) return false;

    // Écriture sélective des données de persistance de l'opérateur
    fwrite(ctx->operator_name, sizeof(char), 50, file);
    fwrite(&ctx->current_level_idx, sizeof(int), 1, file);
    fwrite(&ctx->surcharge, sizeof(int), 1, file);

    fclose(file);
    return true;
}
*/
