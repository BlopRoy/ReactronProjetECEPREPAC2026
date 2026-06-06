#ifndef GESTION_NIVEAUX_H
#define GESTION_NIVEAUX_H

#include "type.h"

void load_level_config(GameContext *ctx, int level_num);
void check_game_conditions(GameContext *ctx);
void handle_level_fail(GameContext *ctx);

#endif