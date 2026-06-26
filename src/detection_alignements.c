#include "detection_alignements.h"

bool detect_and_mark(GameContext *ctx) {
    bool found = false;
    
    bool trigger_bomb_for_type[CELL_COUNT] = { false };

    for (int r = 0; r < MAP_ROWS; r++) {
        int match_count = 1;
        for (int c = 1; c <= MAP_COLS; c++) {
            if (c < MAP_COLS && ctx->matrix[r][c] == ctx->matrix[r][c-1] && ctx->matrix[r][c] != CELL_EMPTY) {
                match_count++;
            } else {
                if (match_count >= 3) {
                    found = true;
                    CellType type_aligne = ctx->matrix[r][c-1];
                    
                    for (int k = c - match_count; k < c; k++) {
                        ctx->marked[r][k] = 1;
                    }
                    if (match_count >= 5) {
                        trigger_bomb_for_type[type_aligne] = true;
                    }
                }
                match_count = 1;
            }
        }
    }

    for (int c = 0; c < MAP_COLS; c++) {
        int match_count = 1;
        for (int r = 1; r <= MAP_ROWS; r++) {
            if (r < MAP_ROWS && ctx->matrix[r][c] == ctx->matrix[r-1][c] && ctx->matrix[r][c] != CELL_EMPTY) {
                match_count++;
            } else {
                if (match_count >= 3) {
                    found = true;
                    CellType type_aligne = ctx->matrix[r-1][c];
                    
                    for (int k = r - match_count; k < r; k++) {
                        ctx->marked[k][c] = 1;
                    }
                    if (match_count >= 5) {
                        trigger_bomb_for_type[type_aligne] = true;
                    }
                }
                match_count = 1;
            }
        }
    }

    for (int t = 1; t < CELL_COUNT; t++) {
        if (trigger_bomb_for_type[t]) {
            for (int r = 0; r < MAP_ROWS; r++) {
                for (int c = 0; c < MAP_COLS; c++) {
                    if (ctx->matrix[r][c] == (CellType)t) {
                        ctx->marked[r][c] = 1;
                    }
                }
            }
        }
    }

    return found;
}

int resolve_suppression(GameContext *ctx) {
    int total_destroyed = 0;
    
    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            if (ctx->marked[r][c] == 1) {
                CellType current_type = ctx->matrix[r][c];
                
                if (current_type > CELL_EMPTY && current_type < CELL_COUNT) {
                    ctx->level.cur_energy[current_type]++;
                }
                
                ctx->matrix[r][c] = CELL_EMPTY;
                ctx->marked[r][c] = 0;
                total_destroyed++;
            }
        }
    }
    return total_destroyed;
}
