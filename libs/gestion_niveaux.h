#ifndef GESTION_NIVEAUX_H
#define GESTION_NIVEAUX_H

#include "type.h"

// Charge la configuration du niveau demandé
void load_level_config(GameContext *ctx, int level_num);

// Vérifie si le joueur a gagné ou perdu
void check_game_conditions(GameContext *ctx);

// Gère les conséquences d'un échec de niveau
void handle_level_fail(GameContext *ctx);

#endif
