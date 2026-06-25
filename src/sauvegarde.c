#include "sauvegarde.h"
#include <stdio.h>
#include <string.h>

#define SAVE_FILE "save.txt"

int load_save(GameContext *ctx) {
    FILE *file = fopen(SAVE_FILE, "r");
    if (!file) {
        perror("Impossible d'ouvrir le fichier de sauvegarde en lecture");
        return 1;
    }

    int result = fscanf(file,
        "Name1:%49s Level1:%d Surcharge1:%d\n"
        "Name2:%49s Level2:%d Surcharge2:%d\n"
        "Name3:%49s Level3:%d Surcharge3:%d\n",
        ctx->save.operator_name_1, &ctx->save.current_level_idx_1, &ctx->save.surcharge_1,
        ctx->save.operator_name_2, &ctx->save.current_level_idx_2, &ctx->save.surcharge_2,
        ctx->save.operator_name_3, &ctx->save.current_level_idx_3, &ctx->save.surcharge_3
    );

    fclose(file);

    if (result != 9) {
        printf("Erreur : Fichier de sauvegarde corrompu.\n");
        return 1;
    }

    return 0;
}

void init_save(GameContext *ctx) {
    strcpy(ctx->save.operator_name_1, "Empty");
    strcpy(ctx->save.operator_name_2, "Empty");
    strcpy(ctx->save.operator_name_3, "Empty");

    ctx->save.surcharge_1 = ctx->save.surcharge_2 = ctx->save.surcharge_3 = 0;
    ctx->save.current_level_idx_1 = ctx->save.current_level_idx_2 = ctx->save.current_level_idx_3 = 1;
    
    load_save(ctx);
}

bool is_file_created(void){
    FILE *file = fopen(SAVE_FILE, "r");

    if (!file) {
        file = fopen(SAVE_FILE, "w");
        if (!file) {
            perror("Impossible de créer le fichier de sauvegarde");
            return false;
        }
        fclose(file);
        return true;
    }

    fclose(file);
    return true;
}

bool save_progress(GameContext *ctx)
{
    if (!is_file_created()) return false;

    FILE *file = fopen(SAVE_FILE, "w");
    if (!file) {
        perror("Impossible d'ouvrir le fichier de sauvegarde");
        return false;
    }

    int found = 0;

    // 1. UPDATE si le joueur existe déjà
    if (strcmp(ctx->operator_name, ctx->save.operator_name_1) == 0) {
        ctx->save.current_level_idx_1 = ctx->current_level_idx;
        ctx->save.surcharge_1 = ctx->surcharge;
        found = 1;
    }
    else if (strcmp(ctx->operator_name, ctx->save.operator_name_2) == 0) {
        ctx->save.current_level_idx_2 = ctx->current_level_idx;
        ctx->save.surcharge_2 = ctx->surcharge;
        found = 1;
    }
    else if (strcmp(ctx->operator_name, ctx->save.operator_name_3) == 0) {
        ctx->save.current_level_idx_3 = ctx->current_level_idx;
        ctx->save.surcharge_3 = ctx->surcharge;
        found = 1;
    }

    // 2. SI NOUVEAU JOUEUR → on remplit un slot vide
    if (!found) {
        if (strcmp(ctx->save.operator_name_1, "Empty") == 0) {
            strcpy(ctx->save.operator_name_1, ctx->operator_name);
            ctx->save.current_level_idx_1 = ctx->current_level_idx;
            ctx->save.surcharge_1 = ctx->surcharge;
        }
        else if (strcmp(ctx->save.operator_name_2, "Empty") == 0) {
            strcpy(ctx->save.operator_name_2, ctx->operator_name);
            ctx->save.current_level_idx_2 = ctx->current_level_idx;
            ctx->save.surcharge_2 = ctx->surcharge;
        }
        else if (strcmp(ctx->save.operator_name_3, "Empty") == 0) {
            strcpy(ctx->save.operator_name_3, ctx->operator_name);
            ctx->save.current_level_idx_3 = ctx->current_level_idx;
            ctx->save.surcharge_3 = ctx->surcharge;
        }
    }

    // 3. écriture fichier
    fprintf(file,
        "Name1:%s Level1:%d Surcharge1:%d\n"
        "Name2:%s Level2:%d Surcharge2:%d\n"
        "Name3:%s Level3:%d Surcharge3:%d\n",
        ctx->save.operator_name_1,
        ctx->save.current_level_idx_1,
        ctx->save.surcharge_1,
        ctx->save.operator_name_2,
        ctx->save.current_level_idx_2,
        ctx->save.surcharge_2,
        ctx->save.operator_name_3,
        ctx->save.current_level_idx_3,
        ctx->save.surcharge_3
    );

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
        "Name1:%49s Level1:%d Surcharge1:%d\n"
        "Name2:%49s Level2:%d Surcharge2:%d\n"
        "Name3:%49s Level3:%d Surcharge3:%d\n",
        ctx->save.operator_name_1,
        &ctx->save.current_level_idx_1,
        &ctx->save.surcharge_1,

        ctx->save.operator_name_2,
        &ctx->save.current_level_idx_2,
        &ctx->save.surcharge_2,

        ctx->save.operator_name_3,
        &ctx->save.current_level_idx_3,
        &ctx->save.surcharge_3
    );
    fclose(file);

    if (result != 9) {
        printf("Erreur : sauvegarde corrompue\n");
        return false;
    }

    // trouver le bon slot
    if (strcmp(ctx->operator_name, ctx->save.operator_name_1) == 0) {
        ctx->current_level_idx = ctx->save.current_level_idx_1;
        ctx->surcharge = ctx->save.surcharge_1;
    }
    else if (strcmp(ctx->operator_name, ctx->save.operator_name_2) == 0) {
        ctx->current_level_idx = ctx->save.current_level_idx_2;
        ctx->surcharge = ctx->save.surcharge_2;
    }
    else if (strcmp(ctx->operator_name, ctx->save.operator_name_3) == 0) {
        ctx->current_level_idx = ctx->save.current_level_idx_3;
        ctx->surcharge = ctx->save.surcharge_3;
    }

    return true;
}
/*
#include "sauvegarde.h"
#include <stdio.h>
#include <string.h>

#define SAVE_FILE "save.txt"

void init_save(GameSave *s) {
    strcpy(s->operator_name_1, "Empty");
    strcpy(s->operator_name_2, "Empty");
    strcpy(s->operator_name_3, "Empty");

    s->surcharge_1 = s->surcharge_2 = s->surcharge_3 = 0;
    s->current_level_idx_1 = s->current_level_idx_2 = s->current_level_idx_3 = 1;
}

bool is_file_created(void){
    FILE *file = fopen(SAVE_FILE, "r");

    if (!file) {
        file = fopen(SAVE_FILE, "w");
        if (!file) {
            perror("Impossible de créer le fichier de sauvegarde");
            return false;
        }
        fclose(file);
        return true;
    }

    fclose(file);
    return true;
}
bool save_progress(const GameContext *ctx, GameSave *ctxs)
{
    if (!is_file_created()) return false;

    FILE *file = fopen(SAVE_FILE, "w");
    if (!file) {
        perror("Impossible d'ouvrir le fichier de sauvegarde");
        return false;
    }

    int found = 0;

    // 1. UPDATE si le joueur existe déjà
    if (strcmp(ctx->operator_name, ctxs->operator_name_1) == 0) {
        ctxs->current_level_idx_1 = ctx->current_level_idx;
        ctxs->surcharge_1 = ctx->surcharge;
        found = 1;
    }
    else if (strcmp(ctx->operator_name, ctxs->operator_name_2) == 0) {
        ctxs->current_level_idx_2 = ctx->current_level_idx;
        ctxs->surcharge_2 = ctx->surcharge;
        found = 1;
    }
    else if (strcmp(ctx->operator_name, ctxs->operator_name_3) == 0) {
        ctxs->current_level_idx_3 = ctx->current_level_idx;
        ctxs->surcharge_3 = ctx->surcharge;
        found = 1;
    }

    // 2. SI NOUVEAU JOUEUR → on remplit un slot vide
    if (!found) {
        if (strcmp(ctxs->operator_name_1, "Empty") == 0) {
            strcpy(ctxs->operator_name_1, ctx->operator_name);
            ctxs->current_level_idx_1 = ctx->current_level_idx;
            ctxs->surcharge_1 = ctx->surcharge;
        }
        else if (strcmp(ctxs->operator_name_2, "Empty") == 0) {
            strcpy(ctxs->operator_name_2, ctx->operator_name);
            ctxs->current_level_idx_2 = ctx->current_level_idx;
            ctxs->surcharge_2 = ctx->surcharge;
        }
        else if (strcmp(ctxs->operator_name_3, "Empty") == 0) {
            strcpy(ctxs->operator_name_3, ctx->operator_name);
            ctxs->current_level_idx_3 = ctx->current_level_idx;
            ctxs->surcharge_3 = ctx->surcharge;
        }
    }

    // 3. écriture fichier
    fprintf(file,
        "Name1:%s Level1:%d Surcharge1:%d\n"
        "Name2:%s Level2:%d Surcharge2:%d\n"
        "Name3:%s Level3:%d Surcharge3:%d\n",
        ctxs->operator_name_1,
        ctxs->current_level_idx_1,
        ctxs->surcharge_1,
        ctxs->operator_name_2,
        ctxs->current_level_idx_2,
        ctxs->surcharge_2,
        ctxs->operator_name_3,
        ctxs->current_level_idx_3,
        ctxs->surcharge_3
    );

    fclose(file);
    return true;
}

bool load_progress(GameContext *ctx, GameSave *ctxs) {
    FILE *file = fopen(SAVE_FILE, "r");
    if (!file) {
        perror("Impossible de lire la sauvegarde");
        return false;
    }

    int result = fscanf(file,
        "Name1:%49s Level1:%d Surcharge1:%d\n"
        "Name2:%49s Level2:%d Surcharge2:%d\n"
        "Name3:%49s Level3:%d Surcharge3:%d\n",
        ctxs->operator_name_1,
        &ctxs->current_level_idx_1,
        &ctxs->surcharge_1,

        ctxs->operator_name_2,
        &ctxs->current_level_idx_2,
        &ctxs->surcharge_2,

        ctxs->operator_name_3,
        &ctxs->current_level_idx_3,
        &ctxs->surcharge_3
    );
    fclose(file);

    if (result != 9) {
        printf("Erreur : sauvegarde corrompue\n");
        return false;
    }

    // trouver le bon slot
    if (strcmp(ctx->operator_name, ctxs->operator_name_1) == 0) {
        ctx->current_level_idx = ctxs->current_level_idx_1;
        ctx->surcharge = ctxs->surcharge_1;
    }
    else if (strcmp(ctx->operator_name, ctxs->operator_name_2) == 0) {
        ctx->current_level_idx = ctxs->current_level_idx_2;
        ctx->surcharge = ctxs->surcharge_2;
    }
    else if (strcmp(ctx->operator_name, ctxs->operator_name_3) == 0) {
        ctx->current_level_idx = ctxs->current_level_idx_3;
        ctx->surcharge = ctxs->surcharge_3;
    }

    return true;
}
*/