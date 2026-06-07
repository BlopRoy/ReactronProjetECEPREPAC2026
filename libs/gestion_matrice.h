#ifndef GESTION_MATRICE_H
#define GESTION_MATRICE_H

#include <stdbool.h>
#include "type.h"

void init_matrix(GameContext *ctx);
bool swap_cells(GameContext *ctx, int x1, int y1, int x2, int y2);
bool apply_gravity(GameContext *ctx);
void refill_top(GameContext *ctx);
void regenerate_if_blocked(GameContext *ctx);

// regarde s'il reste des coups possibles 
bool has_valid_moves(GameContext *ctx);
bool has_empty_cell(GameContext * ctx);

#endif