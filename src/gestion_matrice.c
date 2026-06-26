#include "gestion_matrice.h"
#include "detection_alignements.h"
#include <stdlib.h>

static bool check_immediate_match(GameContext *ctx, int r, int c, CellType type) {
    if (c >= 2 && ctx->matrix[r][c-1] == type && ctx->matrix[r][c-2] == type) return true;
    if (r >= 2 && ctx->matrix[r-1][c] == type && ctx->matrix[r-2][c] == type) return true;
    return false;
}

void init_matrix(GameContext *ctx) {
    bool valid_grid = false;
    
    while (!valid_grid) {
        for (int r = 0; r < MAP_ROWS; r++) {
            for (int c = 0; c < MAP_COLS; c++) {
                CellType t;
                int attempts = 0;
                do {
                    t = (CellType)(rand() % (CELL_COUNT - 1) + 1);
                    attempts++;
                } while (check_immediate_match(ctx, r, c, t) && attempts < 10);
                
                ctx->matrix[r][c] = t;
                ctx->marked[r][c] = 0;
            }
        }
        
        if (has_valid_moves(ctx)) {
            valid_grid = true;
        }
    }
    
    ctx->cursor_x = 0;
    ctx->cursor_y = 0;
    ctx->selected_x = -1;
    ctx->selected_y = -1;
}

bool swap_cells(GameContext *ctx, int x1, int y1, int x2, int y2) {
    if (x1 < 0 || x1 >= MAP_COLS || y1 < 0 || y1 >= MAP_ROWS ||
        x2 < 0 || x2 >= MAP_COLS || y2 < 0 || y2 >= MAP_ROWS) {
        return false;
    }

    CellType temp = ctx->matrix[y1][x1];
    ctx->matrix[y1][x1] = ctx->matrix[y2][x2];
    ctx->matrix[y2][x2] = temp;

    if (detect_and_mark(ctx)) {
        for (int r = 0; r < MAP_ROWS; r++) {
            for (int c = 0; c < MAP_COLS; c++) {
                ctx->marked[r][c] = 0;
            }
        }
        return true; 
    }

    temp = ctx->matrix[y1][x1];
    ctx->matrix[y1][x1] = ctx->matrix[y2][x2];
    ctx->matrix[y2][x2] = temp;
    
    return false;
}

bool apply_gravity(GameContext *ctx) {
    bool moved = false;
    for (int c = 0; c < MAP_COLS; c++) {
        for (int r = MAP_ROWS - 2; r >= 0; r--) {
            if (ctx->matrix[r][c] != CELL_EMPTY && ctx->matrix[r+1][c] == CELL_EMPTY) {
                int current_r = r;
                while (current_r + 1 < MAP_ROWS && ctx->matrix[current_r+1][c] == CELL_EMPTY) {
                    ctx->matrix[current_r+1][c] = ctx->matrix[current_r][c];
                    ctx->matrix[current_r][c] = CELL_EMPTY;
                    current_r++;
                    moved = true;
                }
            }
        }
    }
    return moved;
}

void refill_top(GameContext *ctx) {
    for (int c = 0; c < MAP_COLS; c++) {
        if (ctx->matrix[0][c] == CELL_EMPTY) {
            ctx->matrix[0][c] = (CellType)(rand() % (CELL_COUNT - 1) + 1);
        }
    }
}

bool has_valid_moves(GameContext *ctx) {
    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            if (c + 1 < MAP_COLS) {
                CellType t = ctx->matrix[r][c];
                ctx->matrix[r][c] = ctx->matrix[r][c+1];
                ctx->matrix[r][c+1] = t;
                
                bool match = detect_and_mark(ctx);
                
                t = ctx->matrix[r][c];
                ctx->matrix[r][c] = ctx->matrix[r][c+1];
                ctx->matrix[r][c+1] = t;
                
                if (match) {
                    for (int i = 0; i < MAP_ROWS; i++)
                        for (int j = 0; j < MAP_COLS; j++) ctx->marked[i][j] = 0;
                    return true;
                }
            }
            if (r + 1 < MAP_ROWS) {
                CellType t = ctx->matrix[r][c];
                ctx->matrix[r][c] = ctx->matrix[r+1][c];
                ctx->matrix[r+1][c] = t;
                
                bool match = detect_and_mark(ctx);
                
                t = ctx->matrix[r][c];
                ctx->matrix[r][c] = ctx->matrix[r+1][c];
                ctx->matrix[r+1][c] = t;
                
                if (match) {
                    for (int i = 0; i < MAP_ROWS; i++)
                        for (int j = 0; j < MAP_COLS; j++) ctx->marked[i][j] = 0;
                    return true;
                }
            }
        }
    }
    return false;
}

void regenerate_if_blocked(GameContext *ctx) {
    if (!has_valid_moves(ctx)) {
        init_matrix(ctx);
    }
}

bool has_empty_cell(GameContext * ctx) {
    for (int r = 0; r  < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            CellType t = ctx->matrix[r][c];
            if(t == CELL_EMPTY) return true; 
        }
    }
    return false;
}
